/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipFileChanged.h"
#include "../Utilities/Fixtures/FileSystem.h"
#include "../Zip/Fixtures/Lib.h"
#include "Fixtures/LibInstance.h"
#include "Fixtures/ZipFactoryAccess.h"
#include "test/gtest.h"
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
	using ::ZipDirFs::Components::ZipFileChanged;

	void ZipFileChangedTest::TearDown()
	{
		ZipFactoryAccess::getArchivesByPath().clear();
		ZipFactoryAccess::getArchivesByData().clear();
	}

	void ZipFileChangedExpectArchive(FileSystem& fs, Lib& lib, LibInstance& data,
		const boost::filesystem::path& path, time_t changed)
	{
		EXPECT_CALL(fs, last_write_time(Eq(ByRef(path)))).WillRepeatedly(Return(changed));
		EXPECT_CALL(lib, open(Eq(ByRef(path)))).WillRepeatedly(Return(&data));
	}

	const std::string ZipFileChangedExpectPopulateArchive(Lib& lib, LibInstance& data)
	{
		const std::string entryName =
			std::string("entry") + std::to_string(::Test::rand(UINT32_MAX));
		EXPECT_CALL(lib, get_num_entries(&data)).WillRepeatedly(Return(1));
		EXPECT_CALL(lib, get_name(&data, 0)).WillRepeatedly(Return(entryName));
		EXPECT_CALL(lib, close(&data)).WillRepeatedly(Return());
		return entryName;
	}

	void ZipFileChangedExpectEntry(Lib& lib, LibInstance& data, std::string name, time_t changed)
	{
		auto stat = ::ZipDirFs::Zip::Base::Stat(0, name, ::Test::rand(UINT32_MAX), changed, false);
		EXPECT_CALL(lib, stat(&data, name)).WillRepeatedly(Return(stat));
	}

	TEST_F(ZipFileChangedTest, Init)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		LibInstance data;
		ZipFileChangedExpectArchive(fs, lib, data, p, std::time_t(0));
		ZipFileChangedExpectPopulateArchive(lib, data);
		ZipFileChanged dc(p, "");
		ASSERT_FALSE(dc());
	}

	TEST_F(ZipFileChangedTest, Same)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::time_t origin(::Test::rand(UINT32_MAX));
		LibInstance data;
		ZipFileChangedExpectArchive(fs, lib, data, p, origin);
		ZipFileChangedExpectPopulateArchive(lib, data);
		ZipFileChanged dc(p, "");
		ASSERT_TRUE(dc());
		ASSERT_FALSE(dc());
	}

	TEST_F(ZipFileChangedTest, Futur)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::time_t origin(::Test::rand(UINT32_MAX));
		std::time_t futur(::Test::rand(UINT32_MAX));
		while (origin == futur)
		{
			futur = ::Test::rand(UINT32_MAX);
		}
		if (origin > futur)
		{
			std::swap(origin, futur);
		}
		LibInstance data;
		ZipFileChangedExpectArchive(fs, lib, data, p, futur);
		ZipFileChangedExpectPopulateArchive(lib, data);
		ZipFileChanged dc(p, "");
		reinterpret_cast<ZipFileChangedAccess*>(&dc)->set_lastPathChanged(origin);
		ASSERT_TRUE(dc());
	}

	TEST_F(ZipFileChangedTest, Past)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::time_t origin(::Test::rand(UINT32_MAX));
		std::time_t past(::Test::rand(UINT32_MAX));
		while (origin == past)
		{
			past = ::Test::rand(UINT32_MAX);
		}
		if (origin < past)
		{
			std::swap(origin, past);
		}
		LibInstance data;
		ZipFileChangedExpectArchive(fs, lib, data, p, past);
		ZipFileChangedExpectPopulateArchive(lib, data);
		ZipFileChanged dc(p, "");
		reinterpret_cast<ZipFileChangedAccess*>(&dc)->set_lastPathChanged(origin);
		ASSERT_TRUE(dc());
	}

	TEST_F(ZipFileChangedTest, Get)
	{
		std::time_t expected(::Test::rand(UINT32_MAX));
		ZipFileChanged dc("", "");
		reinterpret_cast<ZipFileChangedAccess*>(&dc)->set_lastPathChanged(expected);
		reinterpret_cast<ZipFileChangedAccess*>(&dc)->set_state(0);
		ASSERT_EQ((std::time_t)dc, expected);
	}

	TEST_F(ZipFileChangedTest, ItemInit)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		LibInstance data;
		ZipFileChangedExpectArchive(fs, lib, data, p, std::time_t(0));
		const auto name = ZipFileChangedExpectPopulateArchive(lib, data);
		ZipFileChangedExpectEntry(lib, data, name, std::time_t(0));
		ZipFileChanged dc(p, name);
		ASSERT_FALSE(dc());
	}

	TEST_F(ZipFileChangedTest, ItemSame)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::time_t origin(::Test::rand(UINT32_MAX));
		LibInstance data;
		ZipFileChangedExpectArchive(fs, lib, data, p, std::time_t(0));
		const auto name = ZipFileChangedExpectPopulateArchive(lib, data);
		ZipFileChangedExpectEntry(lib, data, name, origin);
		ZipFileChanged dc(p, name);
		ASSERT_TRUE(dc());
		ASSERT_FALSE(dc());
	}

	TEST_F(ZipFileChangedTest, ItemFutur)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::time_t origin(::Test::rand(UINT32_MAX));
		std::time_t futur(::Test::rand(UINT32_MAX));
		while (origin == futur)
		{
			futur = ::Test::rand(UINT32_MAX);
		}
		if (origin > futur)
		{
			std::swap(origin, futur);
		}
		LibInstance data;
		ZipFileChangedExpectArchive(fs, lib, data, p, std::time_t(0));
		const auto name = ZipFileChangedExpectPopulateArchive(lib, data);
		ZipFileChangedExpectEntry(lib, data, name, futur);
		ZipFileChanged dc(p, name);
		reinterpret_cast<ZipFileChangedAccess*>(&dc)->set_lastItemChanged(origin);
		ASSERT_TRUE(dc());
	}

	TEST_F(ZipFileChangedTest, ItemPast)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::time_t origin(::Test::rand(UINT32_MAX));
		std::time_t past(::Test::rand(UINT32_MAX));
		while (origin == past)
		{
			past = ::Test::rand(UINT32_MAX);
		}
		if (origin < past)
		{
			std::swap(origin, past);
		}
		LibInstance data;
		ZipFileChangedExpectArchive(fs, lib, data, p, 0);
		const auto name = ZipFileChangedExpectPopulateArchive(lib, data);
		ZipFileChangedExpectEntry(lib, data, name, past);
		ZipFileChanged dc(p, name);
		reinterpret_cast<ZipFileChangedAccess*>(&dc)->set_lastItemChanged(origin);
		ASSERT_TRUE(dc());
	}

	TEST_F(ZipFileChangedTest, ItemGet)
	{
		std::time_t expected(::Test::rand(UINT32_MAX));
		ZipFileChanged dc("", "");
		reinterpret_cast<ZipFileChangedAccess*>(&dc)->set_lastItemChanged(expected);
		reinterpret_cast<ZipFileChangedAccess*>(&dc)->set_state(2);
		ASSERT_EQ((std::time_t)dc, expected);
	}

	TEST_F(ZipFileChangedTest, GetTime)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::time_t expected(::Test::rand(UINT32_MAX));
		LibInstance data;
		ZipFileChangedExpectArchive(fs, lib, data, p, expected);
		ZipFileChangedExpectPopulateArchive(lib, data);
		ASSERT_EQ(ZipFileChanged::getTime(p, ""), expected);
	}

	TEST_F(ZipFileChangedTest, GetTimeItem)
	{
		FileSystem fs;
		Lib lib;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::time_t expected(::Test::rand(UINT32_MAX));
		LibInstance data;
		ZipFileChangedExpectArchive(fs, lib, data, p, std::time_t(0));
		const auto name = ZipFileChangedExpectPopulateArchive(lib, data);
		ZipFileChangedExpectEntry(lib, data, name, expected);
		ASSERT_EQ(ZipFileChanged::getTime(p, name), expected);
	}
} // namespace Test::ZipDirFs::Components
