/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "NativeTimePolicy.h"
#include "ZipDirFs/Fuse/NativeTimePolicy.h"
#include "test/gtest.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Fuse
{
	using ::testing::Return;

	namespace{
		struct NTPDerived{};
		struct NativeTimePolicyMock : public ::ZipDirFs::Fuse::NativeTimePolicy<NTPDerived>
		{
			MOCK_CONST_METHOD0(getChangeTime, std::time_t());
			MOCK_CONST_METHOD0(getModificationTime, std::time_t());
		};
	}

	TEST(NativeTimePolicyTest, change_time)
	{
		std::time_t expected(::Test::rand(UINT32_MAX));
		NativeTimePolicyMock ntp;
		EXPECT_CALL(ntp, getChangeTime()).WillOnce(Return(expected));
		ASSERT_EQ(ntp.change_time().tv_sec, expected);
	}

	TEST(NativeTimePolicyTest, access_time)
	{
		std::time_t expected(::Test::rand(UINT32_MAX));
		NativeTimePolicyMock ntp;
		EXPECT_CALL(ntp, getModificationTime()).WillOnce(Return(expected));
		ASSERT_EQ(ntp.access_time().tv_sec, expected);
	}

	TEST(NativeTimePolicyTest, modification_time)
	{
		std::time_t expected(::Test::rand(UINT32_MAX));
		NativeTimePolicyMock ntp;
		EXPECT_CALL(ntp, getModificationTime()).WillOnce(Return(expected));
		ASSERT_EQ(ntp.modification_time().tv_sec, expected);
	}
} // namespace Test::ZipDirFs::Fuse
