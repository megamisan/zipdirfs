/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Zip/Lib.h"
#include "ZipDirFs/Zip/Exception.h"
#include <boost/filesystem.hpp>
#include <fcntl.h>
#include <zip.h>

#define ZIP_STAT_EXPECTED_FIELDS (ZIP_STAT_NAME | ZIP_STAT_INDEX | ZIP_STAT_SIZE | ZIP_STAT_MTIME)
namespace ZipDirFs::Zip
{
	namespace
	{
		struct LibWrapper : public Lib
		{
			static Base::Lib* open(const path&);
			static void close(Base::Lib* const);
			static std::uint64_t get_num_entries(Base::Lib* const) noexcept;
			static Base::Stat stat(Base::Lib* const, const std::string&);
			static Base::Stat stat_index(Base::Lib* const, const std::uint64_t);
			static std::string get_name(Base::Lib* const, std::uint64_t);
			static Base::Lib::File* fopen_index(Base::Lib* const, std::uint64_t);
			static std::uint64_t fread(Base::Lib::File* const, void*, std::uint64_t);
			static std::int64_t ftell(Base::Lib::File* const);
			static void fclose(Base::Lib::File* const);
		};

		struct LibData : public Base::Lib
		{
			LibData(::zip_t*);
			::zip_t* const zip;
		};

		struct FileData : public Base::Lib::File
		{
			FileData(::zip_file_t*);
			::zip_file_t* const file;
		};

		struct ZipError : public zip_error_t
		{
			ZipError(::zip_uint64_t);
			ZipError(::zip*);
			~ZipError();

		protected:
			const ::zip_t* source;
		};

		struct Version
		{
			Version(const char*);
			bool operator<(const Version&) const;

		private:
			const int maj, min, rev;
		};

		bool haveFdBug = Version(::zip_libzip_version()) < Version("1.5.2");
	} // namespace

	LibData::LibData(::zip* const z) : zip(z) {}
	FileData::FileData(::zip_file_t* f) : file(f) {}

	ZipError::ZipError(::zip_uint64_t error) : source(nullptr)
	{
		::zip_error_init_with_code(this, error);
	}
	ZipError::ZipError(::zip_t* s) : zip_error_t(*::zip_get_error(s)), source(s) {}
	ZipError::~ZipError()
	{
		if (source != nullptr)
		{
			::zip_error_fini(this);
		}
		else
		{
			::zip_error_clear(const_cast<::zip_t*>(source));
		}
	}

	Version::Version(const char* version) : maj(0), min(0), rev(0)
	{
		sscanf(version, "%d.%d.%d", const_cast<int*>(&maj), const_cast<int*>(&min),
			const_cast<int*>(&rev));
	}
	bool Version::operator<(const Version& other) const
	{
		return (maj < other.maj)
			|| (maj == other.maj && (min < other.min || (min == other.min && rev < other.rev)));
	}

	void Lib::reset()
	{
		assign(Lib::open, LibWrapper::open);
		assign(Lib::close, LibWrapper::close);
		assign(Lib::get_num_entries, LibWrapper::get_num_entries);
		assign(Lib::stat, LibWrapper::stat);
		assign(Lib::stat_index, LibWrapper::stat_index);
		assign(Lib::get_name, LibWrapper::get_name);
		assign(Lib::fopen_index, LibWrapper::fopen_index);
		assign(Lib::fread, LibWrapper::fread);
		assign(Lib::ftell, LibWrapper::ftell);
		assign(Lib::fclose, LibWrapper::fclose);
	}

	std::function<Base::Lib*(const path&)> Lib::open;
	std::function<void(Base::Lib* const)> Lib::close;
	std::function<std::uint64_t(Base::Lib* const) noexcept> Lib::get_num_entries;
	std::function<Base::Stat(Base::Lib* const, const std::string&)> Lib::stat;
	std::function<Base::Stat(Base::Lib* const, const std::uint64_t)> Lib::stat_index;
	std::function<std::string(Base::Lib* const, std::uint64_t)> Lib::get_name;
	std::function<Base::Lib::File*(Base::Lib* const, std::uint64_t)> Lib::fopen_index;
	std::function<std::uint64_t(Base::Lib::File* const, void*, std::uint64_t)> Lib::fread;
	std::function<std::int64_t(Base::Lib::File* const)> Lib::ftell;
	std::function<void(Base::Lib::File* const)> Lib::fclose;

	bool init = []
	{
		Lib::reset();
		return true;
	}();

	Base::Lib* LibWrapper::open(const path& p)
	{
		::zip* zipFile = nullptr;
		int handle = ::open64(p.c_str(), O_RDONLY | O_EXCL | O_NOATIME | O_LARGEFILE);
		if (handle < 0)
		{
			throw Exception::fromErrorno("ZipFile::Zip::Lib::open", errno);
		}
		std::int32_t error;
		if (haveFdBug)
		{
			::close(handle);
			handle = 0;
			if (::boost::filesystem::file_size(p) == 0)
			{
				ZipError err(ZIP_ER_NOZIP);
				throw Exception("ZipFile::Zip::Lib::open", ::zip_error_strerror(&err));
			}
			zipFile = ::zip_open(p.c_str(), ZIP_RDONLY, &error);
			if (zipFile == nullptr)
			{
				ZipError err(error);
				throw Exception("ZipFile::Zip::Lib::open", ::zip_error_strerror(&err));
			}
		}
		else
		{
			zipFile = ::zip_fdopen(handle, 0, &error);
			if (zipFile == nullptr)
			{
				ZipError err(error);
				::close(handle);
				throw Exception("ZipFile::Zip::Lib::open", ::zip_error_strerror(&err));
			}
		}
		return new LibData{zipFile};
	}
	void LibWrapper::close(Base::Lib* const l)
	{
		LibData* lw = reinterpret_cast<decltype(lw)>(l);
		::zip_discard(lw->zip);
		delete lw;
	}

	std::uint64_t LibWrapper::get_num_entries(Base::Lib* const l) noexcept
	{
		LibData* lw = reinterpret_cast<decltype(lw)>(l);
		return zip_get_num_entries(lw->zip, 0);
	}
	Base::Stat LibWrapper::stat(Base::Lib* const l, const std::string& n)
	{
		LibData* lw = reinterpret_cast<decltype(lw)>(l);
		::zip_stat_t stats;
		::zip_stat_init(&stats);
		if (::zip_stat(lw->zip, n.c_str(), 0, &stats) != 0)
		{
			ZipError err(lw->zip);
			throw Exception("ZipFile::Zip::Lib::stat", ::zip_error_strerror(&err));
		}
		if ((stats.valid & ZIP_STAT_EXPECTED_FIELDS) != ZIP_STAT_EXPECTED_FIELDS)
		{
			std::__throw_runtime_error("ZipFile::Zip::Lib::stat: Incomplete stats received.");
		}
		return Base::Stat(stats.index, stats.name, stats.size, stats.mtime,
			stats.comp_method != ZIP_CM_STORE || !(stats.valid & ZIP_STAT_COMP_METHOD));
	}
	Base::Stat LibWrapper::stat_index(Base::Lib* const l, const uint64_t i)
	{
		LibData* lw = reinterpret_cast<decltype(lw)>(l);
		::zip_stat_t stats;
		::zip_stat_init(&stats);
		if (::zip_stat_index(lw->zip, i, 0, &stats) != 0)
		{
			ZipError err(lw->zip);
			throw Exception("ZipFile::Zip::Lib::stat_index", ::zip_error_strerror(&err));
		}
		if ((stats.valid & ZIP_STAT_EXPECTED_FIELDS) != ZIP_STAT_EXPECTED_FIELDS)
		{
			std::__throw_runtime_error("ZipFile::Zip::Lib::stat_index: Incomplete stats received.");
		}
		return Base::Stat(stats.index, stats.name, stats.size, stats.mtime,
			stats.comp_method != ZIP_CM_STORE || !(stats.valid & ZIP_STAT_COMP_METHOD));
	}
	std::string LibWrapper::get_name(Base::Lib* const l, std::uint64_t i)
	{
		LibData* lw = reinterpret_cast<decltype(lw)>(l);
		const char* name = ::zip_get_name(lw->zip, i, 0);
		if (name == nullptr)
		{
			ZipError err(lw->zip);
			throw Exception("ZipFile::Zip::Lib::get_name", ::zip_error_strerror(&err));
		}
		return name;
	}
	Base::Lib::File* LibWrapper::fopen_index(Base::Lib* const l, std::uint64_t i)
	{
		LibData* lw = reinterpret_cast<decltype(lw)>(l);
		::zip_file_t* file = ::zip_fopen_index(lw->zip, i, 0);
		if (file == nullptr)
		{
			ZipError err(lw->zip);
			throw Exception("ZipFile::Zip::Lib::fopen_index", ::zip_error_strerror(&err));
		}
		return new FileData{file};
	}
	std::uint64_t LibWrapper::fread(Base::Lib::File* const f, void* buf, std::uint64_t len)
	{
		FileData* lf = reinterpret_cast<decltype(lf)>(f);
		auto read = ::zip_fread(lf->file, buf, len);
		if (read == -1UL)
		{
			std::__throw_ios_failure("Failure reading file.");
		}
		return read;
	}
	std::int64_t LibWrapper::ftell(Base::Lib::File* const f)
	{
		FileData* lf = reinterpret_cast<decltype(lf)>(f);
		return ::zip_ftell(lf->file);
	}
	void LibWrapper::fclose(Base::Lib::File* const f)
	{
		FileData* lf = reinterpret_cast<decltype(lf)>(f);
		::zip_fclose(lf->file);
		delete lf;
	}

} // namespace ZipDirFs::Zip
