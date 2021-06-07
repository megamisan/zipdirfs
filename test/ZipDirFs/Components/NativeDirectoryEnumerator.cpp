/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "NativeDirectoryEnumerator.h"
#include "../Containers/Fixtures/EntryIteratorEndWrapper.h"
#include "../Containers/Fixtures/EntryIteratorWrapperMock.h"
#include "../Utilities/Fixtures/FileSystem.h"
#include "ZipDirFs/Components/NativeDirectoryEnumerator.h"
#include "test/gtest.h"
#include <boost/filesystem.hpp>
#include <gmock/gmock.h>

namespace Test::ZipDirFs::Components
{
	using ::Test::ZipDirFs::Containers::Fixtures::EntryIteratorEndWrapper;
	using ::Test::ZipDirFs::Containers::Fixtures::EntryIteratorWrapperMock;
	using ::Test::ZipDirFs::Utilities::Fixtures::FileSystem;
	using ::testing::Return;
	using ::testing::ReturnRef;
	using ::ZipDirFs::Components::NativeDirectoryEnumerator;

	ACTION_P(ReturnEndWrapper, endValue) { return new EntryIteratorEndWrapper(endValue); }

	TEST(NativeDirectoryEnumeratorTest, Init)
	{
		FileSystem fs;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::int64_t endValue = ::Test::rand(UINT32_MAX);
		auto endWrapper = new EntryIteratorEndWrapper(endValue);
		EXPECT_CALL(fs, directory_iterator_end()).WillOnce(Return(endWrapper));
		NativeDirectoryEnumerator nde(p);
	}

	TEST(NativeDirectoryEnumeratorTest, Reset)
	{
		FileSystem fs;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::int64_t endValue = ::Test::rand(UINT32_MAX);
		auto wrapperMock = new EntryIteratorWrapperMock();
		EXPECT_CALL(fs, directory_iterator_end()).WillOnce(ReturnEndWrapper(endValue));
		EXPECT_CALL(fs, directory_iterator_from_path(p)).WillOnce(Return(wrapperMock));
		NativeDirectoryEnumerator nde(p);
		nde.reset();
	}

	TEST(NativeDirectoryEnumeratorTest, Next)
	{
		FileSystem fs;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::int64_t endValue = ::Test::rand(UINT32_MAX);
		auto endWrapper = new EntryIteratorEndWrapper(endValue);
		auto wrapperMock = new EntryIteratorWrapperMock();
		EXPECT_CALL(fs, directory_iterator_end()).WillOnce(Return(wrapperMock));
		EXPECT_CALL(*wrapperMock, clone()).WillOnce(Return(endWrapper));
		EXPECT_CALL(*wrapperMock, increment());
		NativeDirectoryEnumerator nde(p);
		nde.next();
	}

	TEST(NativeDirectoryEnumeratorTest, Valid)
	{
		FileSystem fs;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::int64_t endValue = ::Test::rand(UINT32_MAX);
		auto endWrapper = new EntryIteratorEndWrapper(endValue);
		auto wrapperMock = new EntryIteratorWrapperMock();
		EXPECT_CALL(fs, directory_iterator_end()).WillOnce(Return(wrapperMock));
		EXPECT_CALL(*wrapperMock, clone()).WillOnce(Return(endWrapper));
		EXPECT_CALL(*wrapperMock, equals_proxy(endWrapper)).WillOnce(Return(false));
		NativeDirectoryEnumerator nde(p);
		ASSERT_TRUE(nde.valid());
	}

	TEST(NativeDirectoryEnumeratorTest, Current)
	{
		FileSystem fs;
		const boost::filesystem::path p(
			boost::filesystem::path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::int64_t endValue = ::Test::rand(UINT32_MAX);
		std::string currentValue =
			std::string("current") + std::to_string(::Test::rand(UINT32_MAX));
		auto endWrapper = new EntryIteratorEndWrapper(endValue);
		auto wrapperMock = new EntryIteratorWrapperMock();
		EXPECT_CALL(fs, directory_iterator_end()).WillOnce(Return(wrapperMock));
		EXPECT_CALL(*wrapperMock, clone()).WillOnce(Return(endWrapper));
		EXPECT_CALL(*wrapperMock, dereference()).WillOnce(ReturnRef(currentValue));
		NativeDirectoryEnumerator nde(p);
		ASSERT_EQ(nde.current(), currentValue);
	}

} // namespace Test::ZipDirFs::Components
