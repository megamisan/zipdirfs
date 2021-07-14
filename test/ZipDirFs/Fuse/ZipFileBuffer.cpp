/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipFileBuffer.h"
#include "../../fusekit/Fixtures/EntryMock.h"
#include "../Components/Fixtures/LibInstance.h"
#include "../Utilities/Fixtures/FileSystem.h"
#include "../Zip/Fixtures/Lib.h"
#include "Fixtures/FuseDaemonFork.h"
#include "Fixtures/OneChanged.h"
#include "ZipDirFs/Fuse/EntryProxy.h"
#include "ZipDirFs/Fuse/ZipFileBuffer.h"
#include "ZipDirFs/Fuse/ZipTimePolicy.h"
#include "ZipDirFs/Utilities/FileSystem.h"
#include "ZipDirFs/Zip/Factory.h"
#include "test/gtest.h"
#include <boost/filesystem.hpp>
#include <fusekit/basic_file.h>
#include <fusekit/default_permissions.h>
#include <fusekit/no_xattr.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <poll.h>

namespace Test::ZipDirFs::Fuse
{
	using ::testing::_;
	using ::testing::Eq;
	using ::testing::Invoke;
	using ::testing::PrintToString;
	using ::testing::Return;
	using ::testing::StrEq;
	using namespace ::boost;
	using ::Test::fusekit::Fixtures::EntryMock;
	using ::Test::ZipDirFs::Components::Fixtures::LibInstance;
	using ::Test::ZipDirFs::Utilities::Fixtures::FileSystem;
	using ::Test::ZipDirFs::Zip::Fixtures::Lib;

	namespace
	{
		using namespace ::ZipDirFs::Fuse;
		using namespace Fixtures;
		using namespace ::fusekit;

		filesystem::path tempFolderPath()
		{
			return filesystem::temp_directory_path()
				/ filesystem::unique_path("test-%%%%-%%%%-%%%%-%%%%");
		}

		struct StatEntry : public ::ZipDirFs::Zip::Entry
		{
			StatEntry(size_t size);
		};

		StatEntry::StatEntry(size_t size) :
			::ZipDirFs::Zip::Entry(
				std::shared_ptr<::ZipDirFs::Zip::Base::Lib>(nullptr), "stat", false)
		{
			flags[1] = true;
			cachedStat = ::ZipDirFs::Zip::Base::Stat(0, "stat", size, 0, false);
		}

		template <class Derived>
		struct StatZipFileBuffer : public ZipFileBuffer<Derived, StatEntry>
		{
		};

		struct StatZipFile : public basic_file<StatZipFileBuffer, ZipTimePolicy,
								 default_file_permissions, no_xattr>
		{
			StatZipFile(const time_t parent, const time_t modified, const size_t size);
			std::time_t getParentModificationTime() const;
			std::time_t getModificationTime() const;
			std::shared_ptr<StatEntry> entry() const;
			off_t entrySize() const;

		protected:
			const time_t _parent, _modified;
			std::shared_ptr<StatEntry> _entry;
		};

		StatZipFile::StatZipFile(const time_t parent, const time_t modified, const size_t size) :
			_parent(parent), _modified(modified), _entry(new StatEntry(size))
		{
		}
		std::time_t StatZipFile::getParentModificationTime() const { return this->_parent; }
		std::time_t StatZipFile::getModificationTime() const { return this->_modified; }
		std::shared_ptr<StatEntry> StatZipFile::entry() const
		{
			return std::shared_ptr<StatEntry>(this->_entry);
		}
		off_t StatZipFile::entrySize() const { return _entry->stat().getSize(); }

		struct ZipFile : public basic_file<DefaultZipFileBuffer, ZipTimePolicy,
							 default_file_permissions, no_xattr>
		{
			ZipFile(const filesystem::path& zip, const std::string& file);
			std::time_t getParentModificationTime() const;
			std::time_t getModificationTime() const;
			std::shared_ptr<::ZipDirFs::Zip::Entry> entry() const;
			off_t entrySize() const;

		protected:
			const filesystem::path zip;
			const std::string file;
			const off_t size;
		};

		ZipFile::ZipFile(const filesystem::path& zip, const std::string& file) :
			zip(zip), file(file), size(entry()->stat().getSize())
		{
		}
		std::time_t ZipFile::getParentModificationTime() const
		{
			return ::ZipDirFs::Utilities::FileSystem::last_write_time(zip);
		}
		std::time_t ZipFile::getModificationTime() const { return entry()->stat().getChanged(); }
		std::shared_ptr<::ZipDirFs::Zip::Entry> ZipFile::entry() const
		{
			return ::ZipDirFs::Zip::Factory::getInstance().get(zip)->open(file);
		}
		off_t ZipFile::entrySize() const { return size; }

		struct Guard
		{
			Guard(std::function<void()> f) : _f(f){};
			~Guard() { _f(); }
			void finalize()
			{
				_f();
				_f = []() {};
			}

		protected:
			std::function<void()> _f;
		};

		struct SimpleDirectory
		{
			SimpleDirectory(const std::string& itemName, entry* item);
			int readdir(void*, fuse_fill_dir_t, off_t, fuse_file_info&);
			int stat(struct stat&);
			std::unique_ptr<entry> getMock()
			{
				::testing::NiceMock<EntryMock>* em(new ::testing::NiceMock<EntryMock>);
				EXPECT_CALL(*em, child(StrEq(itemName))).WillRepeatedly(Return(item));
				ON_CALL(*em, stat(_)).WillByDefault(Invoke(this, &SimpleDirectory::stat));
				ON_CALL(*em, access(_)).WillByDefault(Return(0));
				ON_CALL(*em, chmod(_)).WillByDefault(Return(-EACCES));
				ON_CALL(*em, open(_)).WillByDefault(Return(-EISDIR));
				ON_CALL(*em, release(_)).WillByDefault(Return(-EISDIR));
				ON_CALL(*em, read(_, _, _, _)).WillByDefault(Return(-EISDIR));
				ON_CALL(*em, write(_, _, _, _)).WillByDefault(Return(-EISDIR));
				ON_CALL(*em, opendir(_)).WillByDefault(Return(0));
				ON_CALL(*em, readdir(_, _, _, _))
					.WillByDefault(Invoke(this, &SimpleDirectory::readdir));
				ON_CALL(*em, releasedir(_)).WillByDefault(Return(0));
				ON_CALL(*em, mknod(_, _, _)).WillByDefault(Return(-EACCES));
				ON_CALL(*em, unlink(_)).WillByDefault(Return(-EACCES));
				ON_CALL(*em, mkdir(_, _)).WillByDefault(Return(-EACCES));
				ON_CALL(*em, rmdir(_)).WillByDefault(Return(-EACCES));
				ON_CALL(*em, flush(_)).WillByDefault(Return(-EISDIR));
				ON_CALL(*em, truncate(_)).WillByDefault(Return(-EISDIR));
				ON_CALL(*em, utimens(_)).WillByDefault(Return(-EACCES));
				ON_CALL(*em, readlink(_, _)).WillByDefault(Return(-EISDIR));
				ON_CALL(*em, symlink(_, _)).WillByDefault(Return(-ENOTSUP));
				ON_CALL(*em, setxattr(_, _, _, _)).WillByDefault(Return(-ENOTSUP));
				ON_CALL(*em, getxattr(_, _, _)).WillByDefault(Return(-ENOTSUP));
				ON_CALL(*em, listxattr(_, _)).WillByDefault(Return(-ENOTSUP));
				ON_CALL(*em, removexattr(_)).WillByDefault(Return(-ENOTSUP));
				return std::unique_ptr<entry>(em);
			}

		protected:
			const std::string itemName;
			::fusekit::entry* const item;
		};

		SimpleDirectory::SimpleDirectory(const std::string& itemName, entry* item) :
			itemName(itemName), item(item)
		{
		}

		int SimpleDirectory::readdir(void* buf, fuse_fill_dir_t filler, off_t, fuse_file_info&)
		{
			filler(buf, ".", nullptr, 0);
			filler(buf, "..", nullptr, 0);
			filler(buf, itemName.c_str(), nullptr, 0);
			return 0;
		}

		int SimpleDirectory::stat(struct stat& stbuf)
		{
			stbuf.st_mode = S_IFDIR | 0755;
			stbuf.st_atim = stbuf.st_ctim = stbuf.st_mtim = (timespec){0};
			stbuf.st_nlink = 2;
			stbuf.st_size = 0;
			return 0;
		}

		struct FileInstance : ::ZipDirFs::Zip::Base::Lib::File
		{
			FileInstance() {}
			~FileInstance() {}
		};

		void GenerateRandomData(char* buffer, std::streamsize len)
		{
			using rand_type = double;
			const std::streamsize last = len - (len % sizeof(rand_type));
			rand_type* dest = reinterpret_cast<decltype(dest)>(buffer);
			rand_type* end = reinterpret_cast<decltype(end)>(buffer) + last / sizeof(rand_type);
			while (dest < end)
			{
				*(dest++) = ::Test::rand();
			}
			if (last != len)
			{
				rand_type value = ::Test::rand();
				std::memcpy(dest, &value, len - last);
			}
		}

		MATCHER_P2(BufferEq, buf, s,
			std::string("is equal to address " + PrintToString((void*)buf) + " for "
				+ PrintToString(s) + " bytes"))
		{
			return memcmp(arg, buf, s) == 0;
		}
	} // namespace

	TEST(ZipFileBufferTest, Stat)
	{
		std::time_t parent(time(NULL)), modified((std::time_t)::Test::rand(parent));
		off_t size(::Test::rand<off_t>(UINT8_MAX, UINT16_MAX));
		filesystem::path mountPoint(tempFolderPath());
		filesystem::create_directory(mountPoint);
		std::string fsName = "ZipFileBufferTestStat";
		Guard rmdir(
			[mountPoint]()
			{
				try
				{
					filesystem::remove(mountPoint);
				}
				catch (boost::filesystem::filesystem_error e)
				{
				}
			});
		StatZipFile szf(parent, modified, size);
		SimpleDirectory sd("stat", &szf);
		int statResult(-1);
		struct stat stbuf
		{
			0
		};
		FuseDaemonFork daemon(
			mountPoint.native(), fsName, std::unique_ptr<::fusekit::entry>(std::move(sd.getMock())),
			[&daemon, &mountPoint, &statResult, &stbuf](std::vector<int> fds) -> void
			{
				Guard readFd([&fds]() { close(fds[0]); });
				Guard unmount(std::bind(std::mem_fn(&FuseDaemonFork::stop), &daemon));
				struct pollfd descriptors[1] = {{fds[0], POLLIN, 0}};
				ppoll(descriptors, 1, nullptr, nullptr);
				if (read(fds[0], &statResult, sizeof(int)) < 0)
				{
					perror("Worker");
				}
				if (read(fds[0], &stbuf, sizeof(struct stat)) < 0)
				{
					perror("Worker");
				}
			},
			"FileStat", {0, 1}, std::vector<std::string>({(mountPoint / "stat").native()}));
		EXPECT_EQ(statResult, 0);
		ASSERT_EQ(stbuf.st_size, size);
		ASSERT_EQ(stbuf.st_ctim.tv_sec, parent);
		ASSERT_EQ(stbuf.st_mtim.tv_sec, modified);
	}

	TEST(ZipFileBufferTest, Read)
	{
		FileSystem fs;
		Lib lib;
		LibInstance data;
		FileInstance file;
		std::time_t parent(time(NULL)), modified((std::time_t)::Test::rand(parent));
		std::streamsize size(::Test::rand<std::streamsize>(10, 50)), offset = 0, resultSize = 0;
		filesystem::path mountPoint(tempFolderPath()),
			facticeZip("zip" + std::to_string(::Test::rand(UINT32_MAX)));
		std::string facticeFile("item" + std::to_string(::Test::rand(UINT32_MAX)));
		::ZipDirFs::Zip::Base::Stat facticeStat(0, facticeFile, size, modified, false);
		filesystem::create_directory(mountPoint);
		std::string fsName = "ZipFileBufferTestRead";
		char *expected = new char[size], *result = new char[size];
		Guard buffer(
			[expected, result]()
			{
				delete[] expected;
				delete[] result;
			});
		Guard rmdir(
			[mountPoint]()
			{
				try
				{
					filesystem::remove(mountPoint);
				}
				catch (boost::filesystem::filesystem_error e)
				{
				}
			});
		GenerateRandomData(expected, size);
		EXPECT_CALL(fs, last_write_time(facticeZip)).WillRepeatedly(Return(parent));
		EXPECT_CALL(lib, open(Eq(facticeZip))).WillRepeatedly(Return(&data));
		EXPECT_CALL(lib, get_num_entries(&data)).WillRepeatedly(Return(1));
		EXPECT_CALL(lib, get_name(&data, 0)).WillRepeatedly(Return(facticeFile.c_str()));
		EXPECT_CALL(lib, close(&data)).WillRepeatedly(Return());
		EXPECT_CALL(lib, stat(&data, Eq(facticeFile))).WillRepeatedly(Return(facticeStat));
		EXPECT_CALL(lib, fopen_index(&data, 0)).WillRepeatedly(Return(&file));
		EXPECT_CALL(lib, fread(&file, _, _))
			.WillRepeatedly(Invoke(
				[expected, &offset, size](
					::ZipDirFs::Zip::Base::Lib::File* file, void* buf, uint64_t len) -> uint64_t
				{
					std::streamsize length = std::min<std::streamsize>(len, size - offset);
					if (length > 0)
					{
						memcpy(buf, expected, length);
						offset += length;
					}
					return length;
				}));
		EXPECT_CALL(lib, fclose(&file)).WillRepeatedly(Return());
		ZipFile zf(facticeZip, facticeFile);
		SimpleDirectory sd("read", &zf);
		FuseDaemonFork daemon(
			mountPoint.native(), fsName, std::unique_ptr<::fusekit::entry>(std::move(sd.getMock())),
			[&daemon, &mountPoint, result, &resultSize](std::vector<int> fds) -> void
			{
				Guard readFd([&fds]() { close(fds[0]); });
				Guard unmount(std::bind(std::mem_fn(&FuseDaemonFork::stop), &daemon));
				struct pollfd descriptors[1] = {{fds[0], POLLIN, 0}};
				ppoll(descriptors, 1, nullptr, nullptr);
				std::streamsize len = -1, offset = 0;
				if (read(fds[0], &len, sizeof(decltype(len))) < 0)
				{
					perror("Worker");
				}
				while (len > 0)
				{
					if (read(fds[0], result + offset, len) < 0)
					{
						perror("Worker");
						break;
					}
					offset += len;
					if (read(fds[0], &len, sizeof(decltype(len))) < 0)
					{
						perror("Worker");
					}
				}
				resultSize = offset;
			},
			"FileRead", {0, 1}, std::vector<std::string>({(mountPoint / "read").native()}));
		EXPECT_EQ(resultSize, size);
		ASSERT_THAT(result, BufferEq(expected, size));
	}
} // namespace Test::ZipDirFs::Fuse
