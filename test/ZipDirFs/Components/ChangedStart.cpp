/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ChangedStart.h"
#include "test/gtest.h"
#include <boost/filesystem.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Components
{
	using ::testing::Return;
	using ::ZipDirFs::Components::ChangedProxy;
	using ::ZipDirFs::Components::ChangedStart;

	namespace
	{
		struct ChangedMock : public ::ZipDirFs::Containers::Helpers::Changed
		{
			MOCK_METHOD0(changed, bool());
			MOCK_CONST_METHOD0(time, std::time_t());
			bool operator()() { return changed(); }
			operator std::time_t() const { return time(); }
		};
	} // namespace

	TEST(ChangedStartTest, Init)
	{
		ChangedStart* changedStart;
		std::shared_ptr<ChangedProxy> proxy(new ChangedProxy(
			std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed>(std::unique_ptr<ChangedStart>(
				changedStart = new ChangedStart([]() -> std::time_t { return 0; },
					[]() -> std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed> {
						return std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed>(
							std::unique_ptr<ChangedMock>());
					},
					proxy)))));
		EXPECT_EQ(ChangedStartAccess::get_lastChanged(*changedStart), 0);
		EXPECT_EQ(ChangedStartAccess::get_called(*changedStart), false);
	}

	TEST(ChangedStartTest, ChangedFalse)
	{
		std::unique_ptr<ChangedMock> changedMock(new ChangedMock());
		EXPECT_CALL(*changedMock, changed()).WillOnce(Return(false));
		std::shared_ptr<ChangedProxy> proxy(new ChangedProxy(
			std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed>(std::unique_ptr<ChangedStart>(
				new ChangedStart([]() -> std::time_t { return 0; },
					[&changedMock]() -> std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed> {
						return std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed>(
							std::move(changedMock));
					},
					proxy)))));
		EXPECT_EQ((*proxy)(), false);
		EXPECT_EQ(changedMock, nullptr);
	}

	TEST(ChangedStartTest, ChangedTrue)
	{
		std::unique_ptr<ChangedMock> changedMock(new ChangedMock());
		EXPECT_CALL(*changedMock, changed()).WillOnce(Return(true));
		std::shared_ptr<ChangedProxy> proxy(new ChangedProxy(
			std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed>(std::unique_ptr<ChangedStart>(
				new ChangedStart([]() -> std::time_t { return 0; },
					[&changedMock]() -> std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed> {
						return std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed>(
							std::move(changedMock));
					},
					proxy)))));
		EXPECT_EQ((*proxy)(), true);
		EXPECT_EQ(changedMock, nullptr);
	}

	TEST(ChangedStartTest, TimeFirst)
	{
		std::time_t expected(::Test::rand(UINT32_MAX));
		ChangedStart* changedStart;
		std::shared_ptr<ChangedProxy> proxy(new ChangedProxy(
			std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed>(std::unique_ptr<ChangedStart>(
				changedStart = new ChangedStart([expected]() -> std::time_t { return expected; },
					[]() -> std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed> {
						return std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed>(
							std::unique_ptr<ChangedMock>());
					},
					proxy)))));
		EXPECT_EQ((time_t)(*proxy), expected);
		EXPECT_EQ(ChangedStartAccess::get_lastChanged(*changedStart), expected);
		EXPECT_EQ(ChangedStartAccess::get_called(*changedStart), true);
	}

	TEST(ChangedStartTest, TimeOther)
	{
		std::time_t expected(::Test::rand(UINT32_MAX));
		ChangedStart* changedStart;
		std::shared_ptr<ChangedProxy> proxy(new ChangedProxy(
			std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed>(std::unique_ptr<ChangedStart>(
				changedStart = new ChangedStart([expected]() -> std::time_t { return ::Test::rand(UINT32_MAX); },
					[]() -> std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed> {
						return std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed>(
							std::unique_ptr<ChangedMock>());
					},
					proxy)))));
		ChangedStartAccess::set_lastChanged(*changedStart, expected);
		ChangedStartAccess::set_called(*changedStart, true);
		EXPECT_EQ((time_t)(*proxy), expected);
	}
} // namespace Test::ZipDirFs::Components
