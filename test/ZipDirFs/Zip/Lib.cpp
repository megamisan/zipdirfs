/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "Lib.h"
#include "ZipDirFs/Zip/Exception.h"
#include "ZipDirFs/Zip/Lib.h"
#include "test/gtest.h"
#include <boost/filesystem.hpp>
#include <cstring>
#include <endian.h>
#include <fstream>

namespace ZipDirFs::Zip::Base
{
	bool operator==(const Stat& s1, const Stat& s2);
} // namespace ZipDirFs::Zip::Base

namespace Test::ZipDirFs::Zip
{
	using ::ZipDirFs::Zip::Lib;

	void LibTest::SetUp()
	{
		libData = nullptr;
		fileData = nullptr;
	}

	void LibTest::TearDown()
	{
		if (fileData != nullptr)
			Lib::fclose(fileData);
		if (libData != nullptr)
			Lib::close(libData);
	}

	namespace
	{
		// from https://github.com/Michaelangel007/crc32
		std::uint32_t crc32_formula_reflect(
			std::size_t len, const void* data, const std::uint32_t POLY = 0xEDB88320U)
		{
			const unsigned char* buffer = (const unsigned char*)data;
			std::uint32_t crc = -1;

			while (len--)
			{
				crc = crc ^ *buffer++;
				for (int bit = 0; bit < 8; bit++)
				{
					if (crc & 1)
						crc = (crc >> 1) ^ POLY;
					else
						crc = (crc >> 1);
				}
			}
			return ~crc;
		}
		std::tuple<std::uint16_t, std::uint16_t> timestampToMsDos(std::time_t time)
		{
			std::tm timeInfo(*std::localtime(&time));
			return {
				timeInfo.tm_mday + ((timeInfo.tm_mon + 1) << 5) + ((timeInfo.tm_year - 80) << 9),
				(timeInfo.tm_sec >> 1) + (timeInfo.tm_min << 5) + (timeInfo.tm_hour << 11)};
		}
		std::time_t msDosToTimestamp(std::tuple<std::uint16_t, std::uint16_t> time)
		{
			std::tm timeInfo{0};
			timeInfo.tm_year = (std::get<0>(time) >> 9) + 80;
			timeInfo.tm_mon = (std::get<0>(time) >> 5) % 16 - 1;
			timeInfo.tm_mday = std::get<0>(time) % 32;
			timeInfo.tm_hour = (std::get<1>(time) >> 11);
			timeInfo.tm_min = (std::get<1>(time) >> 5) % 64;
			timeInfo.tm_sec = (std::get<1>(time) % 32) << 1;
			timeInfo.tm_isdst = -1;
			return std::mktime(&timeInfo);
		}

		std::string tempZipPath()
		{
			return (boost::filesystem::temp_directory_path()
				/ boost::filesystem::unique_path("test-%%%%-%%%%-%%%%-%%%%.zip"))
				.native();
		}

		struct Buffer
		{
			Buffer() : Buffer(0) {}
			Buffer(std::uint64_t);
			~Buffer();
			struct BinaryString
			{
				constexpr BinaryString(const char* d, std::uint64_t l) : data(d), len(l) {}
				std::add_const<const char*>::type data;
				std::add_const<std::uint64_t>::type len;
			};
			operator std::vector<char>();
			Buffer& operator=(Buffer&&);
			Buffer& operator=(Buffer&);
			std::uint64_t off() const { return position; }

		protected:
			void append(const void*, std::uint64_t);
			char* data;
			std::uint64_t size;
			std::uint64_t position;
			template <typename _Tp>
			friend Buffer& operator<<(Buffer&, const _Tp&);
		};
		struct GeneratedZipFile
		{
			using Stat = ::ZipDirFs::Zip::Base::Stat;
			// Compression method, mtime, mdate, crc32, compsize, size, data, name, comment, offset
			using FileHeader = std::tuple<std::uint16_t, std::uint16_t, std::uint16_t,
				std::uint32_t, std::uint32_t, std::uint32_t, std::string, std::string, std::string,
				std::uint32_t>;
			GeneratedZipFile();
			~GeneratedZipFile();
			constexpr const std::string& getZipFilePath() const { return zipFilePath; }
			constexpr const std::string& getValidFileName() const { return validFileName; }
			constexpr const std::string& getGarbageFileName() const { return garbageFileName; }
			constexpr const std::string& getNonExistantFileName() const
			{
				return nonExistantFileName;
			}
			constexpr const std::uint16_t& getNumEntries() const { return numEntries; }
			constexpr const std::uint16_t& getValidIndex() const { return validIndex; }
			constexpr const std::uint16_t& getGarbageIndex() const { return garbageIndex; }
			constexpr const Stat& getStatByName() const { return statByName; }
			constexpr const Stat& getStatByIndex() const { return statByIndex; }
			constexpr const std::vector<char>& getValidContent() const { return validContent; }

		protected:
			std::string zipFilePath;
			std::string validFileName;
			std::string garbageFileName;
			std::string nonExistantFileName;
			std::uint16_t numEntries;
			std::uint16_t validIndex;
			std::uint16_t garbageIndex;
			Stat statByName;
			Stat statByIndex;
			std::vector<char> validContent;

			std::uint16_t FindValidIndex(const std::vector<FileHeader>&);
			std::uint16_t FindGarbageIndex(const std::vector<FileHeader>&);
			bool IsNonExistant(const std::vector<FileHeader>&, const std::string&);
			std::string GenerateData();
			std::string GenerateName();
			std::string GenerateComment();
			std::uint32_t ComputeCrc32(const char*, std::size_t);
			FileHeader GenerateFile();
			std::size_t ComputeSize(const std::vector<FileHeader>&, const std::string&);
			void WriteLocalFile(FileHeader&, Buffer&);
			void WriteCentralFile(FileHeader&, Buffer&);

		private:
		};

		template <typename _Tp>
		Buffer& operator<<(Buffer& b, const _Tp& v)
		{
			static_assert(!std::is_pointer<std::remove_cv_t<_Tp>>::value);
			static_assert(!std::is_function<_Tp>::value);
			b.append(reinterpret_cast<const char*>(&v), sizeof(_Tp));
			return b;
		}
		template <>
		Buffer& operator<<<std::uint16_t>(Buffer& b, const std::uint16_t& v)
		{
			auto lv = htole16(v);
			b.append(&lv, sizeof(lv));
			return b;
		}
		template <>
		Buffer& operator<<<std::uint32_t>(Buffer& b, const std::uint32_t& v)
		{
			auto lv = htole32(v);
			b.append(&lv, sizeof(lv));
			return b;
		}
		template <>
		Buffer& operator<<<std::uint64_t>(Buffer& b, const std::uint64_t& v)
		{
			auto lv = htole64(v);
			b.append(&lv, sizeof(lv));
			return b;
		}
		template <>
		Buffer& operator<<<Buffer::BinaryString>(Buffer& b, const Buffer::BinaryString& v)
		{
			b.append(v.data, v.len);
			return b;
		}

		template <typename T, typename... U>
		void* getAddress(std::function<T(U...)> f)
		{
			typedef T(fnType)(U...);
			fnType** fnPointer = f.template target<fnType*>();
			return (void*)*fnPointer;
		}

		template <typename _Tp>
		struct random_iterator : public std::iterator<std::random_access_iterator_tag, _Tp>
		{
			static_assert(std::is_arithmetic<_Tp>::value);
			random_iterator(std::uint64_t c = 0, std::function<bool(_Tp)> f = nullptr) :
				count(c), filter(f)
			{
				if (c)
					increment();
			}
			random_iterator<_Tp>& operator++()
			{
				if (--count > 0)
				{
					increment();
				}
				return *this;
			}
			const _Tp& operator*() const { return current; }
			bool operator==(const random_iterator<_Tp>& it) const
			{
				return (count == it.count)
					&& (count == 0 || it.count == 0 || getAddress(filter) == getAddress(it.filter));
			}
			ptrdiff_t operator-(const random_iterator<_Tp>& other) { return other.count - count; }

		protected:
			void __attribute__((noinline)) increment()
			{
				do
				{
					current = ::Test::rand(
						std::numeric_limits<_Tp>::min(), std::numeric_limits<_Tp>::max());
				} while (filter != nullptr && !filter(current));
			}
			std::uint64_t count;
			_Tp current;
			std::function<bool(_Tp)> filter;
		};
		template <typename _Tp>
		inline bool operator!=(const random_iterator<_Tp>& a, const random_iterator<_Tp>& b)
		{
			return !(a == b);
		}

		class GTEST_API_ KilledBySigAbrtOrSigSegv
		{
		public:
			bool operator()(int exit_status) const;
		};

		int LibOpenFileCount()
		{
			auto it = ::boost::filesystem::directory_iterator("/proc/self/fd"),
				 endIt = ::boost::filesystem::directory_iterator();
			int count = 0;
			while (it != endIt)
			{
				++count;
				++it;
			}
			return count;
		}
	} // namespace

	Buffer::Buffer(std::uint64_t reservedSize) :
		data(reservedSize ? new char[reservedSize] : nullptr), size(reservedSize), position(0)
	{
	}

	Buffer::~Buffer()
	{
		if (data != nullptr)
			delete[] data;
	}

	Buffer::operator std::vector<char>() { return std::vector<char>(data, data + position); }
	Buffer& Buffer::operator=(Buffer&& b)
	{
		delete[] data;
		data = b.data, b.data = nullptr;
		size = b.size, b.size = 0;
		position = b.position, b.position = 0;
		return *this;
	}
	Buffer& Buffer::operator=(Buffer& b)
	{
		delete[] data;
		data = new char[b.size];
		memcpy(data, b.data, b.position);
		size = b.size, position = b.position;
		return *this;
	}

	void Buffer::append(const void* src, std::uint64_t len)
	{
		len = std::min(len, size - position);
		memcpy(data + position, src, len);
		position += len;
	}

	GeneratedZipFile::GeneratedZipFile()
	{
		zipFilePath = tempZipPath();
		numEntries = ::Test::rand(3, 15);
		std::vector<FileHeader> files(numEntries);
		for (auto& file : files)
		{
			file = GenerateFile();
		}
		validIndex = FindValidIndex(files);
		garbageIndex = FindGarbageIndex(files);
		std::uint64_t currentIndex = -1;
		while (validIndex == (std::uint16_t)-1)
		{
			do
			{
				currentIndex = ::Test::rand(numEntries - 1);
			} while (garbageIndex != (std::uint16_t)-1 && currentIndex == garbageIndex);
			files[currentIndex] = GenerateFile();
			validIndex = FindValidIndex(files);
		}
		while (garbageIndex == (std::uint16_t)-1)
		{
			do
			{
				currentIndex = ::Test::rand(numEntries - 1);
			} while (validIndex != (std::uint16_t)-1 && currentIndex == validIndex);
			files[currentIndex] = GenerateFile();
			garbageIndex = FindGarbageIndex(files);
		}
		validFileName = std::get<7>(files[validIndex]);
		garbageFileName = std::get<7>(files[garbageIndex]);
		do
		{
			nonExistantFileName = GenerateName();
		} while (!IsNonExistant(files, nonExistantFileName));
		statByIndex = Stat(validIndex, validFileName, std::get<5>(files[validIndex]),
			msDosToTimestamp({std::get<2>(files[validIndex]), std::get<1>(files[validIndex])}),
			false);
		statByName = Stat(garbageIndex, std::get<7>(files[garbageIndex]),
			std::get<5>(files[garbageIndex]),
			msDosToTimestamp({std::get<2>(files[garbageIndex]), std::get<1>(files[garbageIndex])}),
			true);
		validContent = std::vector<char>(
			std::get<6>(files[validIndex]).begin(), std::get<6>(files[validIndex]).end());
		std::string comment = GenerateComment();
		Buffer temp(ComputeSize(files, comment));
		for (auto& file : files)
		{
			WriteLocalFile(file, temp);
		}
		std::uint32_t centralOffset = temp.off();
		for (auto& file : files)
		{
			WriteCentralFile(file, temp);
		}
		std::uint32_t centralLength = temp.off() - centralOffset;
		temp << 0x06054b50 // signature
			 << 0U // disk number, central start
			 << numEntries // entries in file
			 << numEntries // total entries
			 << centralLength << centralOffset << (std::uint16_t)comment.length()
			 << Buffer::BinaryString(comment.c_str(), comment.length());
		{
			std::ofstream output(zipFilePath);
			std::vector<char> content(temp);
			output.write(content.data(), content.size());
		}
	}

	GeneratedZipFile::~GeneratedZipFile() { unlink(zipFilePath.c_str()); }

	std::uint16_t GeneratedZipFile::FindValidIndex(const std::vector<FileHeader>& files)
	{
		std::vector<std::uint16_t> valids;
		for (std::uint16_t i = 0; i < files.size(); i++)
		{
			if (std::get<0>(files[i]) == 0)
			{
				valids.push_back(i);
			}
		}
		return valids.size() ? valids[::Test::rand(valids.size() - 1)] : -1;
	}

	std::uint16_t GeneratedZipFile::FindGarbageIndex(const std::vector<FileHeader>& files)
	{
		std::vector<std::uint16_t> invalids;
		for (std::uint16_t i = 0; i < files.size(); i++)
		{
			if (std::get<0>(files[i]) != 0)
			{
				invalids.push_back(i);
			}
		}
		return invalids.size() ? invalids[::Test::rand(invalids.size() - 1)] : -1;
	}

	bool GeneratedZipFile::IsNonExistant(
		const std::vector<FileHeader>& files, const std::string& name)
	{
		bool result = true;
		for (auto& file : files)
		{
			result = result && (std::get<7>(file) != name);
		}
		return result;
	}

	std::string GeneratedZipFile::GenerateData()
	{
		return std::string(random_iterator<std::string::value_type>(::Test::rand(65536, 262144)),
			random_iterator<std::string::value_type>());
	}

	std::string GeneratedZipFile::GenerateName()
	{
		std::string::value_type last = 0;
		return std::string(
			random_iterator<std::string::value_type>(::Test::rand(10, 64),
				[&last](std::string::value_type c) -> bool
				{
					//  !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
					// oo-ooooooo-ooooooooooooooo-o-o--oooooooooooooooooooooooooooo-ooooooooooooooooooooooooooooooo-oo
					bool result = (c == '/' && (last == '.' || (last != '/' && last != 0)))
						|| ((c >= 32)
							&& (std::strchr("\"*:<>?\\|", c)
								== nullptr)); // Windows name restriction with "/" allowed
					if (result)
						last = c;
					return result;
				}),
			random_iterator<std::string::value_type>());
	}

	std::string GeneratedZipFile::GenerateComment()
	{
		return std::string(
			random_iterator<std::string::value_type>(::Test::rand(256),
				[](std::string::value_type c) -> bool { return c == '\n' || c >= 32; }),
			random_iterator<std::string::value_type>());
	}

	std::uint32_t GeneratedZipFile::ComputeCrc32(const char* content, std::size_t length)
	{
		return crc32_formula_reflect(length, content);
	}

	GeneratedZipFile::FileHeader GeneratedZipFile::GenerateFile()
	{
		FileHeader fh;
		bool compressed = !!::Test::rand(1);
		auto mtime = timestampToMsDos(::Test::rand(631152000L, 3221225471));
		std::get<0>(fh) = compressed ? 8 : 0;
		std::get<1>(fh) = std::get<1>(mtime);
		std::get<2>(fh) = std::get<0>(mtime);
		std::get<6>(fh) = GenerateData();
		std::get<3>(fh) = compressed ?
			::Test::rand(UINT32_MAX) :
			  ComputeCrc32(std::get<6>(fh).c_str(), std::get<6>(fh).length());
		std::get<4>(fh) = std::get<6>(fh).length();
		std::get<5>(fh) = compressed ? ::Test::rand(UINT32_MAX) : std::get<4>(fh);
		std::get<7>(fh) = GenerateName();
		std::get<8>(fh) = GenerateComment();
		return fh;
	}

	std::size_t GeneratedZipFile::ComputeSize(
		const std::vector<FileHeader>& files, const std::string& fileComment)
	{
		std::size_t result(0);
		auto end = files.end();
		for (auto it = files.begin(); it != end; ++it)
		{
			// Local base + Central base + name * 2 + comment + data (no extra)
			result += 30 + 46 + std::get<7>(*it).length() * 2 + std::get<8>(*it).length()
				+ std::get<6>(*it).length();
		}
		result += 22 + fileComment.length();
		return result;
	}

	void GeneratedZipFile::WriteLocalFile(FileHeader& fh, Buffer& b)
	{
		std::get<9>(fh) = b.off(); // file offset
		b << 0x04034b50U // signature
		  << 0x1BU // version + flags
		  << std::get<0>(fh) // compression
		  << std::get<1>(fh) // mtime
		  << std::get<2>(fh) // mdate
		  << std::get<3>(fh) // crc32
		  << std::get<4>(fh) // compsize
		  << std::get<5>(fh) // size
		  << std::uint16_t(std::get<7>(fh).length()) // name length
		  << std::uint16_t(0) // extra length
		  << Buffer::BinaryString(std::get<7>(fh).c_str(), std::get<7>(fh).length()) // name
		  << Buffer::BinaryString(std::get<6>(fh).c_str(), std::get<6>(fh).length()); // data
	}

	void GeneratedZipFile::WriteCentralFile(FileHeader& fh, Buffer& b)
	{
		b << 0x02014b50U // signature
		  << 0x1B001BU // versions
		  << std::uint16_t(0) // flags
		  << std::get<0>(fh) // compression
		  << std::get<1>(fh) // mtime
		  << std::get<2>(fh) // mdate
		  << std::get<3>(fh) // crc32
		  << std::get<4>(fh) // compsize
		  << std::get<5>(fh) // size
		  << std::uint16_t(std::get<7>(fh).length()) // name length
		  << std::uint16_t(0) // extra length
		  << std::uint16_t(std::get<8>(fh).length()) // comment length
		  << 0U // disk number + internal attributes
		  << 0U // external attributes
		  << std::get<9>(fh) // offset
		  << Buffer::BinaryString(std::get<7>(fh).c_str(), std::get<7>(fh).length()) // name
		  << Buffer::BinaryString(std::get<8>(fh).c_str(), std::get<8>(fh).length()); // comment
	}

	bool KilledBySigAbrtOrSigSegv::operator()(int exit_status) const
	{
#if defined(GTEST_KILLED_BY_SIGNAL_OVERRIDE_)
		{
			bool result;
			if (GTEST_KILLED_BY_SIGNAL_OVERRIDE_(SIGABRT, exit_status, &result)
				|| GTEST_KILLED_BY_SIGNAL_OVERRIDE_(SIGSEGV, exit_status, &result))
			{
				return result;
			}
		}
#endif // defined(GTEST_KILLED_BY_SIGNAL_OVERRIDE_)
		return WIFSIGNALED(exit_status)
			&& (WTERMSIG(exit_status) == SIGABRT || WTERMSIG(exit_status) == SIGSEGV);
	}

	TEST_F(LibTest, OpenNonExistant)
	{
		int startFdCount = LibOpenFileCount();
		EXPECT_THROW(libData = Lib::open(tempZipPath()), ::ZipDirFs::Zip::Exception);
		EXPECT_EQ(LibOpenFileCount(), startFdCount);
	}

	TEST_F(LibTest, OpenNonZip)
	{
		int startFdCount = LibOpenFileCount();
		auto tempName(tempZipPath());
		{
			std::ofstream output(tempName);
			std::size_t size = ::Test::rand(4096, 1048576);
			while (size--)
			{
				output << ::Test::rand<char, int, int>(0, 255);
			}
		}
		EXPECT_THROW(libData = Lib::open(tempName), ::ZipDirFs::Zip::Exception);
		EXPECT_EQ(LibOpenFileCount(), startFdCount);
		unlink(tempName.c_str());
	}

	TEST_F(LibTest, OpenNonZipEmptyFile)
	{
		int startFdCount = LibOpenFileCount();
		auto tempName(tempZipPath());
		{
			std::ofstream output(tempName);
		}
		EXPECT_THROW(libData = Lib::open(tempName), ::ZipDirFs::Zip::Exception);
		EXPECT_EQ(LibOpenFileCount(), startFdCount);
		unlink(tempName.c_str());
	}

	TEST_F(LibTest, OpenZip)
	{
		int startFdCount = LibOpenFileCount();
		GeneratedZipFile file;
		EXPECT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		EXPECT_EQ(LibOpenFileCount(), startFdCount + 1);
	}

	TEST_F(LibTest, Close)
	{
		int startFdCount = LibOpenFileCount();
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		ASSERT_EXIT(
			{
				char data[sizeof(decltype(libData)) + sizeof(std::intptr_t)];
				memcpy(data, libData, sizeof(data));
				Lib::close(reinterpret_cast<decltype(libData)>(data));
			},
			KilledBySigAbrtOrSigSegv(), "");
		Lib::close(libData);
		libData = nullptr;
		EXPECT_EQ(LibOpenFileCount(), startFdCount);
	}

	TEST_F(LibTest, GetNumEntries)
	{
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		EXPECT_EQ(Lib::get_num_entries(libData), file.getNumEntries());
	}

	TEST_F(LibTest, StatsNotFound)
	{
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		EXPECT_THROW(Lib::stat(libData, file.getNonExistantFileName()), ::ZipDirFs::Zip::Exception);
	}

	TEST_F(LibTest, StatsOk)
	{
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		EXPECT_EQ(Lib::stat(libData, file.getGarbageFileName()), file.getStatByName());
	}

	TEST_F(LibTest, StatIndexNotFound)
	{
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		EXPECT_THROW(
			Lib::stat_index(libData, ::Test::rand(file.getNumEntries(), (std::uint16_t)255)),
			::ZipDirFs::Zip::Exception);
	}

	TEST_F(LibTest, StatIndexOk)
	{
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		EXPECT_EQ(Lib::stat_index(libData, file.getValidIndex()), file.getStatByIndex());
	}

	TEST_F(LibTest, GetNameValid)
	{
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		EXPECT_EQ(Lib::get_name(libData, file.getValidIndex()), file.getValidFileName());
	}

	TEST_F(LibTest, GetNameInvalid)
	{
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		EXPECT_THROW(Lib::get_name(libData, ::Test::rand(file.getNumEntries(), (std::uint16_t)255)),
			::ZipDirFs::Zip::Exception);
	}

	TEST_F(LibTest, FopenIndexValid)
	{
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		EXPECT_NO_THROW(fileData = Lib::fopen_index(libData, file.getValidIndex()));
	}

	TEST_F(LibTest, FopenIndexInvalid)
	{
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		EXPECT_THROW(fileData = Lib::fopen_index(
						 libData, ::Test::rand(file.getNumEntries(), (std::uint16_t)255)),
			::ZipDirFs::Zip::Exception);
	}

	TEST_F(LibTest, FreadOk)
	{
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		ASSERT_NO_THROW(fileData = Lib::fopen_index(libData, file.getValidIndex()));
		char buffer[4096];
		std::size_t read;
		EXPECT_NO_THROW(read = Lib::fread(fileData, buffer, 4096));
		EXPECT_PRED2([read](const char* a, const char* b) -> bool
			{ return std::memcmp(a, b, read) == 0; },
			buffer, file.getValidContent().data());
	}

	TEST_F(LibTest, FreadNok)
	{
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		ASSERT_NO_THROW(fileData = Lib::fopen_index(libData, file.getGarbageIndex()));
		char buffer[4096];
		EXPECT_THROW(Lib::fread(fileData, buffer, 4096), std::ios::failure);
	}

	TEST_F(LibTest, FseekOk)
	{
		GeneratedZipFile file;
		::ZipDirFs::Zip::Base::Stat stat;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		ASSERT_NO_THROW(fileData = Lib::fopen_index(libData, file.getValidIndex()));
		ASSERT_NO_THROW(stat = Lib::stat_index(libData, file.getValidIndex()));
		EXPECT_TRUE(Lib::fseek(fileData, ::Test::rand(10, stat.getSize() - 10), SEEK_SET));
	}

	TEST_F(LibTest, FseekNok)
	{
		GeneratedZipFile file;
		::ZipDirFs::Zip::Base::Stat stat;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		ASSERT_NO_THROW(fileData = Lib::fopen_index(libData, file.getGarbageIndex()));
		ASSERT_NO_THROW(stat = Lib::stat_index(libData, file.getGarbageIndex()));
		EXPECT_FALSE(Lib::fseek(fileData, ::Test::rand(10, stat.getSize() - 10), SEEK_SET));
	}

	TEST_F(LibTest, FtellOk)
	{
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		ASSERT_NO_THROW(fileData = Lib::fopen_index(libData, file.getValidIndex()));
		EXPECT_EQ(Lib::ftell(fileData), 0);
	}

	TEST_F(LibTest, FtellNok)
	{
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		ASSERT_NO_THROW(fileData = Lib::fopen_index(libData, file.getGarbageIndex()));
		EXPECT_EQ(Lib::ftell(fileData), -1);
	}

	TEST_F(LibTest, Fclose)
	{
		GeneratedZipFile file;
		ASSERT_NO_THROW(libData = Lib::open(file.getZipFilePath()));
		ASSERT_NO_THROW(fileData = Lib::fopen_index(libData, file.getValidIndex()));
		EXPECT_EXIT(
			{
				char data[sizeof(decltype(fileData)) + sizeof(std::intptr_t)];
				memcpy(data, fileData, sizeof(data));
				Lib::fclose(reinterpret_cast<decltype(fileData)>(data));
				fileData = nullptr;
			},
			KilledBySigAbrtOrSigSegv(), "");
	}
} // namespace Test::ZipDirFs::Zip
