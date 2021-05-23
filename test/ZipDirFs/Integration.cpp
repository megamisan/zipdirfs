/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "Integration.h"
#include "Components/Fixtures/LibInstance.h"
#include "Fuse/Fixtures/FuseDaemonFork.h"
#include "Utilities/Fixtures/FileSystem.h"
#include "Zip/Fixtures/Lib.h"
#include "ZipDirFs/Fuse/NativeDirectory.h"
#include "ZipDirFs/Fuse/ZipRootDirectory.h"
#include "ZipDirFs/Fuse/ZipDirectory.h"
#include "ZipDirFs/Zip/Exception.h"
#include "ZipDirFs/Components/ZipFileChanged.h"
#include "test/gtest.h"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>
#include <poll.h>
#include <vector>

namespace Test::ZipDirFs
{
	using Components::Fixtures::LibInstance;
	using Fuse::Fixtures::FuseDaemonFork;
	using Utilities::Fixtures::FileSystem;
	using Zip::Fixtures::Lib;
	using ::ZipDirFs::Fuse::NativeDirectory;
	using ::ZipDirFs::Fuse::ZipRootDirectory;
	using ::ZipDirFs::Fuse::ZipDirectory;
	using ::ZipDirFs::Zip::Base::Stat;
	using namespace ::boost;
	using ::testing::_;
	using ::testing::ByRef;
	using ::testing::Eq;
	using ::testing::Invoke;
	using ::testing::Return;
	using ::testing::ReturnNew;
	using ::testing::StrEq;
	using ::testing::Throw;
	using ::testing::WithoutArgs;

	namespace
	{
		filesystem::path tempFolderPath()
		{
			return filesystem::temp_directory_path()
				/ filesystem::unique_path("test-%%%%-%%%%-%%%%-%%%%");
		}

		struct FileInstance : ::ZipDirFs::Zip::Base::Lib::File
		{
			FileInstance() {}
			~FileInstance() {}
		};

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

		typedef std::vector<std::string> strings;

		struct EndIteratorWrapper : public ::ZipDirFs::Containers::EntryIterator::Wrapper
		{
			EndIteratorWrapper();
			~EndIteratorWrapper(){};

		protected:
			Wrapper* clone() const;
			reference dereference() const;
			void increment();
			bool equals(const Wrapper&) const;
			std::remove_reference<reference>::type dummy;
		};

		struct VectorIteratorWrapper : public ::ZipDirFs::Containers::EntryIterator::Wrapper
		{
			VectorIteratorWrapper(strings::const_iterator&& it, strings::const_iterator&& end);
			VectorIteratorWrapper(
				const strings::const_iterator& it, const strings::const_iterator& end);
			~VectorIteratorWrapper(){};

		protected:
			Wrapper* clone() const;
			reference dereference() const;
			void increment();
			bool equals(const Wrapper&) const;
			strings::const_iterator currentIt, endIt;
			friend class EndIteratorWrapper;
		};

		VectorIteratorWrapper::VectorIteratorWrapper(
			strings::const_iterator&& it, strings::const_iterator&& end) :
			currentIt(std::move(it)),
			endIt(std::move(end)){};
		VectorIteratorWrapper::VectorIteratorWrapper(
			const strings::const_iterator& it, const strings::const_iterator& end) :
			currentIt(it),
			endIt(end){};
		::ZipDirFs::Containers::EntryIterator::Wrapper* VectorIteratorWrapper::clone() const
		{
			return new VectorIteratorWrapper(currentIt, endIt);
		}
		VectorIteratorWrapper::reference VectorIteratorWrapper::dereference() const
		{
			return *currentIt;
		}
		void VectorIteratorWrapper::increment() { ++currentIt; }
		bool VectorIteratorWrapper::equals(
			const ::ZipDirFs::Containers::EntryIterator::Wrapper& w) const
		{
			auto wrapper = dynamic_cast<const VectorIteratorWrapper*>(&w);
			return ((wrapper != nullptr) && (wrapper->currentIt == this->currentIt))
				|| ((dynamic_cast<const EndIteratorWrapper*>(&w) != nullptr)
					&& (this->currentIt == this->endIt));
		}

		EndIteratorWrapper::EndIteratorWrapper(){};
		::ZipDirFs::Containers::EntryIterator::Wrapper* EndIteratorWrapper::clone() const
		{
			return new EndIteratorWrapper();
		}
		EndIteratorWrapper::reference EndIteratorWrapper::dereference() const { return dummy; }
		void EndIteratorWrapper::increment() {}
		bool EndIteratorWrapper::equals(
			const ::ZipDirFs::Containers::EntryIterator::Wrapper& w) const
		{
			auto vectorWrapper = dynamic_cast<const VectorIteratorWrapper*>(&w);
			return (dynamic_cast<const EndIteratorWrapper*>(&w) != nullptr)
				|| ((vectorWrapper != nullptr)
					&& (vectorWrapper->currentIt == vectorWrapper->endIt));
		}

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

		std::streamsize ComputeDirectory(const std::string& path)
		{
			return sizeof(std::time_t) * 3 + sizeof(decltype(stat::st_nlink))
				+ sizeof(decltype(stat::st_size)) + sizeof(decltype(stat::st_mode)) + sizeof(short)
				+ path.length();
		}

		std::streamsize ComputeFile(const std::string& path, std::streamsize size)
		{
			return sizeof(std::time_t) * 3 + sizeof(decltype(stat::st_nlink))
				+ sizeof(decltype(stat::st_size)) + sizeof(decltype(stat::st_mode)) + sizeof(short)
				+ path.length() + size;
		}

		std::streamsize ComputeLink(const std::string& path, const std::string& target)
		{
			return sizeof(std::time_t) * 3 + sizeof(decltype(stat::st_nlink))
				+ sizeof(decltype(stat::st_size)) + sizeof(decltype(stat::st_mode)) + sizeof(short)
				+ path.length() + target.length();
		}

		struct IntegrationExchangeFormat
		{
			decltype(stat::st_mode) mode;
			std::time_t change, access, modified;
			decltype(stat::st_nlink) links;
			decltype(stat::st_size) size;
			short len;
		} __attribute__((packed));

		void WriteDirectory(std::vector<char>& content, const std::time_t change,
			const std::time_t modified, const decltype(stat::st_nlink) nlink,
			const std::string& path)
		{
			IntegrationExchangeFormat ief{
				S_IFDIR | 0555, change, modified, modified, nlink, 0, (short)path.length()};
			content.insert(content.end(), (char*)&ief, (char*)(&ief + 1));
			content.insert(content.end(), path.begin(), path.end());
		}

		void WriteFile(std::vector<char>& content, const std::time_t change,
			const std::time_t modified, const decltype(stat::st_size) size, const std::string& path,
			const char* buffer)
		{
			IntegrationExchangeFormat ief{
				S_IFREG | 0444, change, modified, modified, 1, size, (short)path.length()};
			content.insert(content.end(), (char*)&ief, (char*)(&ief + 1));
			content.insert(content.end(), path.begin(), path.end());
			content.insert(content.end(), buffer, buffer + size);
		}

		void WriteLink(std::vector<char>& content, const std::time_t change,
			const std::time_t modified, const std::string& path, const std::string& target)
		{
			IntegrationExchangeFormat ief{S_IFLNK | 0777, change, modified, modified, 1,
				(decltype(stat::st_size))target.length(), (short)path.length()};
			content.insert(content.end(), (char*)&ief, (char*)(&ief + 1));
			content.insert(content.end(), path.begin(), path.end());
			content.insert(content.end(), target.begin(), target.end());
		}

		std::string integrationReadable(const std::vector<char>& v)
		{
			std::map<decltype(stat::st_mode), char> types{
				{S_IFDIR, 'd'}, {S_IFREG, 'f'}, {S_IFLNK, 'l'}};
			std::stringstream ss;
			IntegrationExchangeFormat ief;
			std::string path, target;
			const char *buffer, *current = v.data(), *end = current + v.size();
			ss << std::setfill('0') << std::right;
			while (current < end)
			{
				if (current + sizeof(ief) > end)
				{
					ss << "**Read error (record): need " << sizeof(ief) << " characters, "
					   << (end - current) << " availables.";
					break;
				}
				memcpy(&ief, current, sizeof(ief));
				current += sizeof(ief);
				ss << (types.find(ief.mode & S_IFMT) != types.end() ? types[ief.mode & S_IFMT] :
																		' ')
				   << std::oct << std::setw(4) << (ief.mode & ALLPERMS) << ' ' << std::hex
				   << std::setw(sizeof(std::time_t) * 2) << ief.change << ' '
				   << std::setw(sizeof(std::time_t) * 2) << ief.access << ' '
				   << std::setw(sizeof(std::time_t) * 2) << ief.modified << ' ' << std::dec
				   << ief.links << ' ' << ief.size << ' ';
				if (current + ief.len > end)
				{
					ss << "\n**Read error (name): need " << ief.len << " characters, "
					   << (end - current) << " availables.";
					break;
				}
				path = std::string(current, current + ief.len);
				current += ief.len;
				ss << path;
				if (S_ISREG(ief.mode) || S_ISLNK(ief.mode))
				{
					if (current + ief.size > end)
					{
						ss << "\n**Read error (content): need " << ief.size << " characters, "
						   << (end - current) << " availables.";
						break;
					}
				}
				if (S_ISREG(ief.mode))
				{
					buffer = current;
					current += ief.size;
					ss << '\n' << std::hex;
					while (buffer < current)
					{
						ss << std::setw(2) << (int)(unsigned char)*(buffer++);
					}
				}
				if (S_ISLNK(ief.mode))
				{
					target = std::string(current, current + ief.size);
					current += ief.size;
					ss << ' ' << target;
				}
				ss << '\n';
			}
			return ss.str();
		}
	} // namespace

	TEST(IntegrationTest, Full)
	{
		// Initialize values
		FileSystem fs;
		Lib lib;
		LibInstance zipInstance1;
		FileInstance itemInstance1, subItemInstance1;
		const std::time_t now(time(NULL)), modifiedRoot((std::time_t)::Test::rand(now)),
			modifiedNormal1((std::time_t)::Test::rand(now)),
			modifiedZip1((std::time_t)::Test::rand(now)),
			modifiedItem1((std::time_t)::Test::rand(now)),
			modifiedSubItem1((std::time_t)::Test::rand(now));
		const std::string normal1("normal" + std::to_string(::Test::rand(UINT32_MAX))),
			zip1("zip" + std::to_string(::Test::rand(UINT32_MAX))),
			item1("item" + std::to_string(::Test::rand(UINT32_MAX))), item1Path(item1),
			subItem1Parent("folder" + std::to_string(::Test::rand(UINT8_MAX))),
			subItem1("subitem" + std::to_string(::Test::rand(UINT32_MAX))),
			subItem1Path(subItem1Parent + "/" + subItem1);
		const filesystem::path mountPoint(tempFolderPath()),
			fakeRoot("/fake" + std::to_string(::Test::rand(UINT32_MAX))),
			fakeNormal1(fakeRoot / normal1), fakeZip1(fakeRoot / zip1);
		const std::streamsize item1Size(::Test::rand<std::streamsize>(UINT8_MAX, UINT16_MAX)),
			subItem1Size(::Test::rand<std::streamsize>(UINT8_MAX, UINT16_MAX));
		const Stat item1Stat(0, item1Path, item1Size, modifiedItem1, false),
			subItem1Stat(1, subItem1Path, subItem1Size, modifiedSubItem1, false);
		filesystem::create_directory(mountPoint);
		const std::string fsName = "IntegrationTestFull";
		Guard rmdir([mountPoint]() {
			try
			{
				filesystem::remove(mountPoint);
			}
			catch (boost::filesystem::filesystem_error e)
			{
			}
		});
		const strings rootDirectoryItems({normal1, zip1});
		char *item1Content = new char[item1Size], *subItem1Content = new char[subItem1Size];
		Guard content([item1Content, subItem1Content]() {
			delete[] item1Content;
			delete[] subItem1Content;
		});
		std::uint64_t item1Offset = 0, subItem1Offset = 0;
		GenerateRandomData(item1Content, item1Size);
		GenerateRandomData(subItem1Content, subItem1Size);
		// Initialize expectations
		EXPECT_CALL(fs, last_write_time(Eq(ByRef(fakeRoot)))).WillRepeatedly(Return(modifiedRoot));
		EXPECT_CALL(fs, last_write_time(Eq(ByRef(fakeNormal1))))
			.WillRepeatedly(Return(modifiedNormal1));
		EXPECT_CALL(fs, last_write_time(Eq(ByRef(fakeZip1)))).WillRepeatedly(Return(modifiedZip1));
		EXPECT_CALL(fs, directory_iterator_from_path(Eq(ByRef(fakeRoot))))
			.WillRepeatedly(ReturnNew<VectorIteratorWrapper>(
				rootDirectoryItems.begin(), rootDirectoryItems.end()));
		EXPECT_CALL(fs, directory_iterator_end()).WillRepeatedly(ReturnNew<EndIteratorWrapper>());
		EXPECT_CALL(fs, status(Eq(ByRef(fakeNormal1))))
			.WillRepeatedly(Return(filesystem::file_status{filesystem::file_type::regular_file}));
		EXPECT_CALL(fs, status(Eq(ByRef(fakeZip1))))
			.WillRepeatedly(Return(filesystem::file_status{filesystem::file_type::regular_file}));
		EXPECT_CALL(lib, open(Eq(ByRef(fakeNormal1))))
			.WillRepeatedly(Throw(::ZipDirFs::Zip::Exception("Mock", "Normal file")));
		EXPECT_CALL(lib, open(Eq(ByRef(fakeZip1)))).WillRepeatedly(Return(&zipInstance1));
		EXPECT_CALL(lib, close(&zipInstance1)).WillRepeatedly(Return());
		EXPECT_CALL(lib, get_num_entries(&zipInstance1)).WillRepeatedly(Return(2));
		EXPECT_CALL(lib, stat(&zipInstance1, Eq(item1Path))).WillRepeatedly(Return(item1Stat));
		EXPECT_CALL(lib, stat(&zipInstance1, Eq(subItem1Path)))
			.WillRepeatedly(Return(subItem1Stat));
		EXPECT_CALL(lib, get_name(&zipInstance1, 0)).WillRepeatedly(Return(item1Path));
		EXPECT_CALL(lib, get_name(&zipInstance1, 1)).WillRepeatedly(Return(subItem1Path));
		EXPECT_CALL(lib, fopen_index(&zipInstance1, 0)).WillRepeatedly(Return(&itemInstance1));
		EXPECT_CALL(lib, fopen_index(&zipInstance1, 1)).WillRepeatedly(Return(&subItemInstance1));
		EXPECT_CALL(lib, fread(&itemInstance1, _, _))
			.WillRepeatedly(Invoke(
				[item1Content, item1Size, &item1Offset](::ZipDirFs::Zip::Base::Lib::File* file,
					void* buf, std::uint64_t len) -> std::uint64_t {
					std::uint64_t length = std::min(len, item1Size - item1Offset);
					if (length > 0)
					{
						memcpy(buf, item1Content + item1Offset, length);
						item1Offset += length;
					}
					return length;
				}));
		EXPECT_CALL(lib, fread(&subItemInstance1, _, _))
			.WillRepeatedly(Invoke([subItem1Content, subItem1Size, &subItem1Offset](
									   ::ZipDirFs::Zip::Base::Lib::File* file, void* buf,
									   std::uint64_t len) -> std::uint64_t {
				std::uint64_t length = std::min(len, subItem1Size - subItem1Offset);
				if (length > 0)
				{
					memcpy(buf, subItem1Content + subItem1Offset, length);
					subItem1Offset += length;
				}
				return length;
			}));
		EXPECT_CALL(lib, fclose(&itemInstance1)).WillRepeatedly(WithoutArgs([&item1Offset]() {
			item1Offset = 0;
		}));
		EXPECT_CALL(lib, fclose(&subItemInstance1)).WillRepeatedly(WithoutArgs([&subItem1Offset]() {
			subItem1Offset = 0;
		}));
		// Initialize expected result
		std::vector<char> expected, result;
		expected.reserve(ComputeDirectory("") + ComputeLink("/" + normal1, fakeNormal1.native())
			+ ComputeDirectory("/" + zip1) + ComputeFile("/" + zip1 + "/" + item1Path, item1Size)
			+ ComputeDirectory("/" + zip1 + "/" + subItem1Parent)
			+ ComputeFile("/" + zip1 + "/" + subItem1Path, subItem1Size));
		WriteDirectory(expected, modifiedRoot, modifiedRoot, 2, "");
		WriteLink(expected, modifiedNormal1, modifiedNormal1, "/" + normal1, fakeNormal1.native());
		WriteDirectory(expected, modifiedZip1, modifiedZip1, 2, "/" + zip1);
		WriteFile(expected, modifiedZip1, modifiedItem1, item1Size, "/" + zip1 + "/" + item1Path,
			item1Content);
		WriteDirectory(expected, modifiedZip1, modifiedZip1, 2, "/" + zip1 + "/" + subItem1Parent);
		WriteFile(expected, modifiedZip1, modifiedSubItem1, subItem1Size,
			"/" + zip1 + "/" + subItem1Path, subItem1Content);
		// Run test
		result.reserve(expected.size());
		FuseDaemonFork daemon(
			mountPoint.native(), fsName,
			std::unique_ptr<::fusekit::entry>(new NativeDirectory(fakeRoot)),
			[&daemon, &mountPoint, &result](std::vector<int> fds) -> void {
				Guard readFd([&fds]() { close(fds[0]); });
				Guard unmount(std::bind(std::mem_fn(&FuseDaemonFork::stop), &daemon));
				struct pollfd descriptors[1] = {{fds[0], POLLIN, 0}};
				ppoll(descriptors, 1, nullptr, nullptr);
				std::streamsize len;
				char buffer[4096];
				while ((len = read(fds[0], &buffer, sizeof(buffer))) > 0)
				{
					result.insert(result.end(), buffer, buffer + len);
				}
				if (len < 0)
				{
					perror("Worker");
				}
			},
			"Explore", {0, 1}, std::vector<std::string>({mountPoint.native()}));
		// Check result
		std::string rResult(integrationReadable(result)), rExpected(integrationReadable(expected));
		ASSERT_EQ(rResult, rExpected);
	}

	TEST(IntegrationTest, NativeDirectoryDirectAccess)
	{
		FileSystem fs;
		const std::time_t now(time(NULL)), modifiedFile((std::time_t)::Test::rand(now));
		const std::string file("file" + std::to_string(::Test::rand(UINT32_MAX)));
		const filesystem::path mountPoint(tempFolderPath()),
			fakeRoot("/fake" + std::to_string(::Test::rand(UINT32_MAX))), fakeFile(fakeRoot / file);
		filesystem::create_directory(mountPoint);
		const std::string fsName = "IntegrationTestNativeDirectoryDirectAccess";
		Guard rmdir([mountPoint]() {
			try
			{
				filesystem::remove(mountPoint);
			}
			catch (boost::filesystem::filesystem_error e)
			{
			}
		});
		EXPECT_CALL(fs, last_write_time(Eq(ByRef(fakeRoot)))).WillRepeatedly(Return(now));
		EXPECT_CALL(fs, last_write_time(Eq(ByRef(fakeFile)))).WillRepeatedly(Return(modifiedFile));
		EXPECT_CALL(fs, status(Eq(ByRef(fakeFile))))
			.WillRepeatedly(Return(filesystem::file_status{filesystem::file_type::regular_file}));
		EXPECT_CALL(fs, directory_iterator_end()).WillRepeatedly(ReturnNew<EndIteratorWrapper>());
		int statResult(-1);
		struct stat stbuf
		{
			0
		};
		FuseDaemonFork daemon(
			mountPoint.native(), fsName,
			std::unique_ptr<::fusekit::entry>(new NativeDirectory(fakeRoot)),
			[&daemon, &mountPoint, &statResult, &stbuf](std::vector<int> fds) -> void {
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
			"FileStat", {0, 1}, std::vector<std::string>({(mountPoint / file).native()}));
		EXPECT_EQ(statResult, 0);
		ASSERT_EQ(stbuf.st_mtim.tv_sec, modifiedFile);
	}

	TEST(IntegrationTest, ZipRootDirectoryDirectAccess)
	{
		FileSystem fs;
		Lib lib;
		LibInstance zipInstance;
		const std::time_t now(time(NULL)), modifiedZip((std::time_t)::Test::rand(now)),
			modifiedItem((std::time_t)::Test::rand(now));
		const std::string zip("zip" + std::to_string(::Test::rand(UINT32_MAX))),
			item("item" + std::to_string(::Test::rand(UINT32_MAX))), itemPath(item);
		const filesystem::path mountPoint(tempFolderPath()),
			fakeRoot("/fake" + std::to_string(::Test::rand(UINT32_MAX))), fakeZip(fakeRoot / zip);
		const std::streamsize itemSize(::Test::rand<std::streamsize>(UINT8_MAX, UINT16_MAX));
		const Stat itemStat(0, itemPath, itemSize, modifiedItem, false);
		filesystem::create_directory(mountPoint);
		const std::string fsName = "IntegrationTestZipRootDirectoryDirectAccess";
		Guard rmdir([mountPoint]() {
			try
			{
				filesystem::remove(mountPoint);
			}
			catch (boost::filesystem::filesystem_error e)
			{
			}
		});
		EXPECT_CALL(fs, last_write_time(Eq(ByRef(fakeZip)))).WillRepeatedly(Return(now));
		EXPECT_CALL(lib, open(Eq(ByRef(fakeZip)))).WillRepeatedly(Return(&zipInstance));
		EXPECT_CALL(lib, close(&zipInstance)).WillRepeatedly(Return());
		EXPECT_CALL(lib, get_num_entries(&zipInstance)).WillRepeatedly(Return(1));
		EXPECT_CALL(lib, stat(&zipInstance, Eq(itemPath))).WillRepeatedly(Return(itemStat));
		EXPECT_CALL(lib, get_name(&zipInstance, 0)).WillRepeatedly(Return(itemPath));
		int statResult(-1);
		struct stat stbuf
		{
			0
		};
		FuseDaemonFork daemon(
			mountPoint.native(), fsName,
			std::unique_ptr<::fusekit::entry>(new ZipRootDirectory(fakeZip)),
			[&daemon, &mountPoint, &statResult, &stbuf](std::vector<int> fds) -> void {
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
			"FileStat", {0, 1}, std::vector<std::string>({(mountPoint / item).native()}));
		EXPECT_EQ(statResult, 0);
		ASSERT_EQ(stbuf.st_mtim.tv_sec, modifiedItem);
	}

	TEST(IntegrationTest, ZipDirectoryDirectAccess)
	{
		FileSystem fs;
		Lib lib;
		LibInstance zipInstance;
		const std::time_t now(time(NULL)), modifiedZip((std::time_t)::Test::rand(now)),
			modifiedItem((std::time_t)::Test::rand(now));
		const std::string zip("zip" + std::to_string(::Test::rand(UINT32_MAX))),
			itemParent("folder" + std::to_string(::Test::rand(UINT32_MAX))),
			item("item" + std::to_string(::Test::rand(UINT32_MAX))), itemPath(itemParent + "/" + item);
		const filesystem::path mountPoint(tempFolderPath()),
			fakeRoot("/fake" + std::to_string(::Test::rand(UINT32_MAX))), fakeZip(fakeRoot / zip);
		::ZipDirFs::Containers::EntryGenerator::changed_ptr rootChanged(
			new ::ZipDirFs::Components::ZipFileChanged(fakeZip, "")
		);
		const std::streamsize itemSize(::Test::rand<std::streamsize>(UINT8_MAX, UINT16_MAX));
		const Stat itemStat(0, itemPath, itemSize, modifiedItem, false);
		filesystem::create_directory(mountPoint);
		const std::string fsName = "IntegrationTestZipDirectoryDirectAccess";
		Guard rmdir([mountPoint]() {
			try
			{
				filesystem::remove(mountPoint);
			}
			catch (boost::filesystem::filesystem_error e)
			{
			}
		});
		EXPECT_CALL(fs, last_write_time(Eq(ByRef(fakeZip)))).WillRepeatedly(Return(now));
		EXPECT_CALL(lib, open(Eq(ByRef(fakeZip)))).WillRepeatedly(Return(&zipInstance));
		EXPECT_CALL(lib, close(&zipInstance)).WillRepeatedly(Return());
		EXPECT_CALL(lib, get_num_entries(&zipInstance)).WillRepeatedly(Return(1));
		EXPECT_CALL(lib, stat(&zipInstance, Eq(itemPath))).WillRepeatedly(Return(itemStat));
		EXPECT_CALL(lib, get_name(&zipInstance, 0)).WillRepeatedly(Return(itemPath));
		int statResult(-1);
		struct stat stbuf
		{
			0
		};
		FuseDaemonFork daemon(
			mountPoint.native(), fsName,
			std::unique_ptr<::fusekit::entry>(new ZipDirectory(fakeZip, itemParent + "/", rootChanged)),
			[&daemon, &mountPoint, &statResult, &stbuf](std::vector<int> fds) -> void {
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
			"FileStat", {0, 1}, std::vector<std::string>({(mountPoint / item).native()}));
		EXPECT_EQ(statResult, 0);
		ASSERT_EQ(stbuf.st_mtim.tv_sec, modifiedItem);
	}
} // namespace Test::ZipDirFs
