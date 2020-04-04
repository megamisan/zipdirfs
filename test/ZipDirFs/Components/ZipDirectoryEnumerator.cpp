/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirectoryEnumerator.h"
#include "../Utilities/Fixtures/FileSystem.h"
#include "../Zip/Fixtures/Lib.h"
#include "Fixtures/LibInstance.h"
#include "Fixtures/ZipFactoryAccess.h"
#include "test/gtest.h"
#include <ZipDirFs/Components/ZipDirectoryEnumerator.h>
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Components
{
	using Fixtures::LibInstance;
	using Fixtures::ZipFactoryAccess;
	using ::Test::ZipDirFs::Utilities::Fixtures::FileSystem;
	using ::Test::ZipDirFs::Zip::Fixtures::Lib;
	using ::testing::_;
	using ::testing::ByRef;
	using ::testing::Eq;
	using ::testing::Return;
	using ::ZipDirFs::Components::ZipDirectoryEnumerator;

	void ZipDirectoryEnumeratorTest::TearDown()
	{
		ZipFactoryAccess::getArchivesByPath().clear();
		ZipFactoryAccess::getArchivesByData().clear();
	}

	void ZipDirectoryEnumeratorExpectArchive(FileSystem& fs, Lib& lib, LibInstance& data,
		const boost::filesystem::path& path, time_t changed)
	{
		EXPECT_CALL(fs, last_write_time(Eq(ByRef(path)))).WillRepeatedly(Return(changed));
		EXPECT_CALL(lib, open(Eq(ByRef(path)))).WillRepeatedly(Return(&data));
	}

	const std::string ZipDirectoryEnumeratorExpectPopulateArchive(Lib& lib, LibInstance& data)
	{
		const std::string entryName =
			std::string("entry") + std::to_string(::Test::rand(UINT32_MAX));
		EXPECT_CALL(lib, get_num_entries(&data)).WillRepeatedly(Return(1));
		EXPECT_CALL(lib, get_name(&data, 0)).WillRepeatedly(Return(entryName));
		EXPECT_CALL(lib, close(&data)).WillRepeatedly(Return());
		return entryName;
	}

	TEST_F(ZipDirectoryEnumeratorTest, Init)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		LibInstance data;
		ZipDirectoryEnumeratorExpectArchive(fs, lib, data, p, std::time_t(0));
		EXPECT_CALL(lib, get_num_entries(&data)).WillRepeatedly(Return(0));
		EXPECT_CALL(lib, close(&data)).WillRepeatedly(Return());
		ZipDirectoryEnumerator zde(p, "");
	}

	TEST_F(ZipDirectoryEnumeratorTest, Valid)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		LibInstance data;
		ZipDirectoryEnumeratorExpectArchive(fs, lib, data, p, std::time_t(0));
		ZipDirectoryEnumeratorExpectPopulateArchive(lib, data);
		ZipDirectoryEnumerator zde(p, "");
		ASSERT_TRUE(zde.valid());
	}

	TEST_F(ZipDirectoryEnumeratorTest, Current)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		LibInstance data;
		ZipDirectoryEnumeratorExpectArchive(fs, lib, data, p, std::time_t(0));
		const auto first = ZipDirectoryEnumeratorExpectPopulateArchive(lib, data);
		ZipDirectoryEnumerator zde(p, "");
		ASSERT_EQ(zde.current(), first);
	}

	TEST_F(ZipDirectoryEnumeratorTest, Next)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		LibInstance data;
		ZipDirectoryEnumeratorExpectArchive(fs, lib, data, p, std::time_t(0));
		ZipDirectoryEnumeratorExpectPopulateArchive(lib, data);
		ZipDirectoryEnumerator zde(p, "");
		zde.next();
		ASSERT_FALSE(zde.valid());
	}

	TEST_F(ZipDirectoryEnumeratorTest, Reset)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		LibInstance data;
		ZipDirectoryEnumeratorExpectArchive(fs, lib, data, p, std::time_t(0));
		const auto first = ZipDirectoryEnumeratorExpectPopulateArchive(lib, data);
		ZipDirectoryEnumerator zde(p, "");
		zde.next();
		zde.reset();
		ASSERT_EQ(zde.current(), first);
	}
} // namespace Test::ZipDirFs::Components
