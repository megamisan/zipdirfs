/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "NativeChanged.h"
#include "../Utilities/Fixtures/FileSystem.h"
#include "test/gtest.h"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Components
{
	using ::Test::ZipDirFs::Utilities::Fixtures::FileSystem;
	using ::testing::_;
	using ::testing::ByRef;
	using ::testing::Eq;
	using ::testing::Return;
	using ::ZipDirFs::Components::NativeChanged;

	TEST(NativeChangedTest, Init)
	{
		FileSystem fs;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		EXPECT_CALL(fs, last_write_time(Eq(ByRef(p)))).WillOnce(Return(std::time_t(0)));
		NativeChanged dc(p);
		ASSERT_FALSE(dc());
	}

	TEST(NativeChangedTest, Same)
	{
		FileSystem fs;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::time_t origin(::Test::rand(UINT32_MAX));
		EXPECT_CALL(fs, last_write_time(Eq(ByRef(p))))
			.Times(2).WillRepeatedly(Return(origin));
		NativeChanged dc(p);
		ASSERT_TRUE(dc());
		ASSERT_FALSE(dc());
	}

	TEST(NativeChangedTest, Futur)
	{
		FileSystem fs;
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
		EXPECT_CALL(fs, last_write_time(Eq(ByRef(p)))).WillOnce(Return(futur));
		NativeChanged dc(p);
		reinterpret_cast<NativeChangedAccess*>(&dc)->set_lastChanged(origin);
		ASSERT_TRUE(dc());
	}

	TEST(NativeChangedTest, Past)
	{
		FileSystem fs;
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
		EXPECT_CALL(fs, last_write_time(Eq(ByRef(p)))).WillOnce(Return(past));
		NativeChanged dc(p);
		reinterpret_cast<NativeChangedAccess*>(&dc)->set_lastChanged(origin);
		ASSERT_TRUE(dc());
	}

	TEST(NativeChangedTest, Get)
	{
		std::time_t expected(::Test::rand(UINT32_MAX));
		NativeChanged dc("");
		reinterpret_cast<NativeChangedAccess*>(&dc)->set_lastChanged(expected);
		ASSERT_EQ((std::time_t)dc, expected);
	}

} // namespace Test::ZipDirFs::Components
