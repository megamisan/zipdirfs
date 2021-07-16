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
#include <thread>

namespace Test::ZipDirFs::Components
{
	using Fixtures::LibInstance;
	using Fixtures::ZipFactoryAccess;
	using ::Test::ZipDirFs::Utilities::Fixtures::FileSystem;
	using ::Test::ZipDirFs::Zip::Fixtures::Lib;
	using ::testing::_;
	using ::testing::ByRef;
	using ::testing::Eq;
	using ::testing::Expectation;
	using ::testing::Return;
	using ::ZipDirFs::Components::ZipDirectoryEnumerator;

	void ZipDirectoryEnumeratorTest::TearDown()
	{
		ZipFactoryAccess::getArchivesByPath().clear();
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

	TEST_F(ZipDirectoryEnumeratorTest, EmptyArchiveCoverage)
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
		EXPECT_FALSE(zde.valid());
	}

	TEST_F(ZipDirectoryEnumeratorTest, AutoCleanup)
	{
		struct OldValueHolder
		{
			OldValueHolder(std::time_t& t, std::time_t newValue) : target(t), oldValue(t)
			{
				target = newValue;
			}
			~OldValueHolder() { target = oldValue; }

		protected:
			const std::time_t oldValue;
			std::time_t& target;
		};
		struct DummyMock
		{
			MOCK_METHOD0(dummy, void());
		};
		OldValueHolder holder(::ZipDirFs::Components::zipDirectoryEnumeratorDelay, (std::time_t)3);
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p1(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		const boost::filesystem::path p2(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		LibInstance data1, data2;
		DummyMock dummy1, dummy2;
		ZipDirectoryEnumeratorExpectArchive(fs, lib, data1, p1, std::time_t(0));
		ZipDirectoryEnumeratorExpectArchive(fs, lib, data2, p2, std::time_t(0));
		EXPECT_CALL(lib, get_num_entries(&data1)).WillRepeatedly(Return(1));
		EXPECT_CALL(lib, get_num_entries(&data2)).WillRepeatedly(Return(1));
		EXPECT_CALL(lib, get_name(&data1, 0))
			.WillRepeatedly(
				Return(std::string("entry") + std::to_string(::Test::rand(UINT32_MAX))));
		EXPECT_CALL(lib, get_name(&data2, 0))
			.WillRepeatedly(
				Return(std::string("entry") + std::to_string(::Test::rand(UINT32_MAX))));
		Expectation d1 = EXPECT_CALL(dummy1, dummy()).WillOnce(Return());
		Expectation close1 = EXPECT_CALL(lib, close(&data1)).After(d1).WillOnce(Return());
		Expectation d2 = EXPECT_CALL(dummy2, dummy()).After(close1).WillOnce(Return());
		Expectation close2 = EXPECT_CALL(lib, close(&data2)).After(d2).WillOnce(Return());
		{
			ZipDirectoryEnumerator zde1(p1, "");
			zde1.next();
			std::this_thread::sleep_for(std::chrono::milliseconds(1500));
			ZipDirectoryEnumerator zde2(p2, "");
			zde2.next();
			dummy1.dummy();
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			dummy2.dummy();
		}
		std::this_thread::yield();
	}
} // namespace Test::ZipDirFs::Components
