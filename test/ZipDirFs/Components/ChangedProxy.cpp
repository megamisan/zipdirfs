/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ChangedProxy.h"
#include "ZipDirFs/Components/ChangedProxy.h"
#include <boost/filesystem.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Components
{
	using ::testing::Return;
	using ::ZipDirFs::Components::ChangedProxy;

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

	TEST(ChangedProxyTest, Init)
	{
		std::unique_ptr<ChangedMock> changedMock(new ChangedMock());
		std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed> changed(std::move(changedMock));
		ChangedProxy proxy(std::move(changed));
		EXPECT_EQ(changed, nullptr);
	}

	TEST(ChangedProxyTest, Changed)
	{
		std::unique_ptr<ChangedMock> changedMock(new ChangedMock());
		EXPECT_CALL(*changedMock, changed()).WillOnce(Return(false));
		ChangedProxy proxy(
			std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed>(std::move(changedMock)));
		proxy();
	}

	TEST(ChangedProxyTest, Time)
	{
		std::unique_ptr<ChangedMock> changedMock(new ChangedMock());
		EXPECT_CALL(*changedMock, time()).WillOnce(Return(0));
		ChangedProxy proxy(
			std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed>(std::move(changedMock)));
		(std::time_t) proxy;
	}

	TEST(ChangedProxyTest, Swap)
	{
		std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed> changedMock1(
			std::unique_ptr<ChangedMock>(new ChangedMock()));
		auto expected = changedMock1.get();
		ChangedProxy proxy(std::move(changedMock1));
		ASSERT_EQ(changedMock1, nullptr);
		proxy.swap(changedMock1);
		EXPECT_EQ(changedMock1.get(), expected);
	}
} // namespace Test::ZipDirFs::Components
