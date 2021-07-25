/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipTimePolicy.h"
#include "ZipDirFs/Fuse/ZipTimePolicy.h"
#include "test/gtest.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Fuse
{
	using ::testing::Return;

	namespace
	{
		struct ZTPDerived
		{
		};
		struct ZipTimePolicyMock : public ::ZipDirFs::Fuse::ZipTimePolicy<ZTPDerived>
		{
			MOCK_CONST_METHOD0(getParentModificationTime, std::time_t());
			MOCK_CONST_METHOD0(getModificationTime, std::time_t());
		};
	} // namespace

	TEST(ZipTimePolicyTest, change_time)
	{
		std::time_t expected(::Test::rand(UINT32_MAX));
		ZipTimePolicyMock ntp;
		EXPECT_CALL(ntp, getParentModificationTime()).WillOnce(Return(expected));
		ASSERT_EQ(ntp.change_time().tv_sec, expected);
	}

	TEST(ZipTimePolicyTest, access_time)
	{
		std::time_t expected(::Test::rand(UINT32_MAX));
		ZipTimePolicyMock ntp;
		EXPECT_CALL(ntp, getModificationTime()).WillOnce(Return(expected));
		ASSERT_EQ(ntp.access_time().tv_sec, expected);
	}

	TEST(ZipTimePolicyTest, modification_time)
	{
		std::time_t expected(::Test::rand(UINT32_MAX));
		ZipTimePolicyMock ntp;
		EXPECT_CALL(ntp, getModificationTime()).WillOnce(Return(expected));
		ASSERT_EQ(ntp.modification_time().tv_sec, expected);
	}
} // namespace Test::ZipDirFs::Fuse
