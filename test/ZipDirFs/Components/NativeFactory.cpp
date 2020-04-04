/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "NativeFactory.h"
#include "../../fusekit/Fixtures/EntryMock.h"
#include "../Utilities/Fixtures/FileSystem.h"
#include "../Zip/Fixtures/Lib.h"
#include "Fixtures/LibInstance.h"
#include "ZipDirFs/Components/NativeFactory.h"
#include "ZipDirFs/Zip/Exception.h"
#include "test/gtest.h"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Components
{
	using namespace boost::filesystem;
	using ::Test::fusekit::Fixtures::EntryMock;
	using ::Test::ZipDirFs::Utilities::Fixtures::FileSystem;
	using ::Test::ZipDirFs::Zip::Fixtures::Lib;
	using ::testing::_;
	using ::testing::ByRef;
	using ::testing::Eq;
	using ::testing::Return;
	using ::testing::Throw;
	using ::ZipDirFs::Components::NativeFactory;

	struct NativeDirectoryEntryMock : EntryMock
	{
		NativeDirectoryEntryMock(const path& p) : path(p){};
		const struct path path;
	};

	struct NativeSymlinkEntryMock : EntryMock
	{
		NativeSymlinkEntryMock(const path& p) : path(p){};
		const struct path path;
	};

	struct NativeZipEntryMock : EntryMock
	{
		NativeZipEntryMock(const path& p) : path(p){};
		const struct path path;
	};

	typedef NativeFactory<NativeDirectoryEntryMock, NativeSymlinkEntryMock, NativeZipEntryMock>
		TestedFactory;

	TEST(NativeFactory, Directory)
	{
		FileSystem fs;
		const std::string item(std::to_string(::Test::rand(UINT32_MAX)));
		const path parent(path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		EXPECT_CALL(fs, status(parent / item))
			.WillOnce(Return(file_status(file_type::directory_file)));
		TestedFactory factory(parent);
		std::unique_ptr<::fusekit::entry, std::function<void(::fusekit::entry*)>> created(
			factory.create(item), [&factory](::fusekit::entry* p) { factory.destroy(p); });
		auto result = dynamic_cast<NativeDirectoryEntryMock*>(created.get());
		ASSERT_NE(result, nullptr);
		ASSERT_EQ(result->path, parent / item);
	}

	TEST(NativeFactory, File)
	{
		FileSystem fs;
		Lib lib;
		const std::string item(std::to_string(::Test::rand(UINT32_MAX)));
		const path parent(path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		EXPECT_CALL(fs, status(parent / item))
			.WillOnce(Return(file_status(file_type::regular_file)));
		EXPECT_CALL(lib, open(parent / item))
			.WillOnce(Throw(::ZipDirFs::Zip::Exception("example", "throw")));
		TestedFactory factory(parent);
		std::unique_ptr<::fusekit::entry, std::function<void(::fusekit::entry*)>> created(
			factory.create(item), [&factory](::fusekit::entry* p) { factory.destroy(p); });
		auto result = dynamic_cast<NativeSymlinkEntryMock*>(created.get());
		ASSERT_NE(result, nullptr);
		ASSERT_EQ(result->path, parent / item);
	}

	TEST(NativeFactory, Zip)
	{
		FileSystem fs;
		Lib lib;
		Fixtures::LibInstance libInstance;
		const std::string item(std::to_string(::Test::rand(UINT32_MAX)));
		const path parent(path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		EXPECT_CALL(fs, status(parent / item))
			.WillOnce(Return(file_status(file_type::regular_file)));
		EXPECT_CALL(lib, open(parent / item)).WillOnce(Return(&libInstance));
		EXPECT_CALL(lib, close(&libInstance));
		TestedFactory factory(parent);
		std::unique_ptr<::fusekit::entry, std::function<void(::fusekit::entry*)>> created(
			factory.create(item), [&factory](::fusekit::entry* p) { factory.destroy(p); });
		auto result = dynamic_cast<NativeZipEntryMock*>(created.get());
		ASSERT_NE(result, nullptr);
		ASSERT_EQ(result->path, parent / item);
	}

	TEST(NativeFactory, Symlink)
	{
		FileSystem fs;
		const std::string item(std::to_string(::Test::rand(UINT32_MAX)));
		const path parent(path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		EXPECT_CALL(fs, status(parent / item))
			.WillOnce(Return(file_status(file_type::symlink_file)));
		TestedFactory factory(parent);
		std::unique_ptr<::fusekit::entry, std::function<void(::fusekit::entry*)>> created(
			factory.create(item), [&factory](::fusekit::entry* p) { factory.destroy(p); });
		auto result = dynamic_cast<NativeSymlinkEntryMock*>(created.get());
		ASSERT_NE(result, nullptr);
		ASSERT_EQ(result->path, parent / item);
	}

} // namespace Test::ZipDirFs::Components
