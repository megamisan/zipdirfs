/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "EntryListProxyProxy.h"
#include "ZipDirFs/Components/EntryListProxyProxy.h"
#include "test/gtest.h"
#include <boost/filesystem.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Components
{
	using ::testing::Return;
	using ::ZipDirFs::Components::EntryListProxyProxy;

	namespace
	{
		struct EntryListProxyMock : public ::ZipDirFs::Containers::Helpers::EntryListProxy
		{
			MOCK_METHOD0(begin, iterator());
			MOCK_METHOD0(end, iterator());
			MOCK_METHOD3(insert, iterator(iterator, const key_type&, const mapped_type&));
			MOCK_METHOD1(erase, iterator(iterator));
			MOCK_CONST_METHOD1(find, fusekit::entry*(const key_type&));
		};

		struct EntryIteratorWrapperTest : public ::ZipDirFs::Containers::EntryIterator::Wrapper
		{
		public:
			EntryIteratorWrapperTest(std::string);

		protected:
			Wrapper* clone() const;
			reference dereference() const;
			void increment();
			bool equals(const Wrapper&) const;
			const std::string value;
		};
	} // namespace

	EntryIteratorWrapperTest::EntryIteratorWrapperTest(std::string value) : value(value) {}
	::ZipDirFs::Containers::EntryIterator::Wrapper* EntryIteratorWrapperTest::clone() const
	{
		return new EntryIteratorWrapperTest(value);
	}
	EntryIteratorWrapperTest::reference EntryIteratorWrapperTest::dereference() const
	{
		return value;
	};
	void EntryIteratorWrapperTest::increment(){};
	bool EntryIteratorWrapperTest::equals(
		const ::ZipDirFs::Containers::EntryIterator::Wrapper& w) const
	{
		auto wrapper = dynamic_cast<const EntryIteratorWrapperTest*>(&w);
		return wrapper != nullptr && wrapper->value == value;
	}

	TEST(EntryListProxyProxyTest, Init)
	{
		std::unique_ptr<EntryListProxyMock> proxyMock(new EntryListProxyMock());
		std::unique_ptr<::ZipDirFs::Containers::Helpers::EntryListProxy> proxy(
			std::move(proxyMock));
		EntryListProxyProxy proxyInstance(std::move(proxy));
		EXPECT_EQ(proxy, nullptr);
	}

	TEST(EntryListProxyProxyTest, Begin)
	{
		std::unique_ptr<EntryListProxyMock> proxyMock(new EntryListProxyMock());
		auto it = EntryListProxyMock::iterator(
			new EntryIteratorWrapperTest(std::to_string(::Test::rand(UINT32_MAX))));
		EXPECT_CALL(*proxyMock, begin()).WillOnce(Return(it));
		EntryListProxyProxy proxy(
			std::unique_ptr<::ZipDirFs::Containers::Helpers::EntryListProxy>(std::move(proxyMock)));
		EXPECT_EQ(proxy.begin(), it);
	}

	TEST(EntryListProxyProxyTest, End)
	{
		std::unique_ptr<EntryListProxyMock> proxyMock(new EntryListProxyMock());
		auto it = EntryListProxyMock::iterator(
			new EntryIteratorWrapperTest(std::to_string(::Test::rand(UINT32_MAX))));
		EXPECT_CALL(*proxyMock, end()).WillOnce(Return(it));
		EntryListProxyProxy proxy(
			std::unique_ptr<::ZipDirFs::Containers::Helpers::EntryListProxy>(std::move(proxyMock)));
		EXPECT_EQ(proxy.end(), it);
	}

	TEST(EntryListProxyProxyTest, Insert)
	{
		std::unique_ptr<EntryListProxyMock> proxyMock(new EntryListProxyMock());
		auto it = EntryListProxyMock::iterator(
				 new EntryIteratorWrapperTest(std::to_string(::Test::rand(UINT32_MAX)))),
			 it2 = EntryListProxyMock::iterator(
				 new EntryIteratorWrapperTest(std::to_string(::Test::rand(UINT32_MAX))));
		std::string key(std::to_string(::Test::rand(UINT32_MAX)));
		fusekit::entry* entry = reinterpret_cast<fusekit::entry*>(&entry);
		EXPECT_CALL(*proxyMock, insert(it, key, entry)).WillOnce(Return(it2));
		EntryListProxyProxy proxy(
			std::unique_ptr<::ZipDirFs::Containers::Helpers::EntryListProxy>(std::move(proxyMock)));
		EXPECT_EQ(proxy.insert(it, key, entry), it2);
	}

	TEST(EntryListProxyProxyTest, Erase)
	{
		std::unique_ptr<EntryListProxyMock> proxyMock(new EntryListProxyMock());
		auto it = EntryListProxyMock::iterator(
				 new EntryIteratorWrapperTest(std::to_string(::Test::rand(UINT32_MAX)))),
			 it2 = EntryListProxyMock::iterator(
				 new EntryIteratorWrapperTest(std::to_string(::Test::rand(UINT32_MAX))));
		EXPECT_CALL(*proxyMock, erase(it)).WillOnce(Return(it2));
		EntryListProxyProxy proxy(
			std::unique_ptr<::ZipDirFs::Containers::Helpers::EntryListProxy>(std::move(proxyMock)));
		EXPECT_EQ(proxy.erase(it), it2);
	}

	TEST(EntryListProxyProxyTest, Find)
	{
		std::unique_ptr<EntryListProxyMock> proxyMock(new EntryListProxyMock());
		std::string key(std::to_string(::Test::rand(UINT32_MAX)));
		fusekit::entry* entry = reinterpret_cast<fusekit::entry*>(&entry);
		EXPECT_CALL(*proxyMock, find(key)).WillOnce(Return(entry));
		EntryListProxyProxy proxy(
			std::unique_ptr<::ZipDirFs::Containers::Helpers::EntryListProxy>(std::move(proxyMock)));
		EXPECT_EQ(proxy.find(key), entry);
	}

	TEST(EntryListProxyProxyTest, Swap)
	{
		std::unique_ptr<::ZipDirFs::Containers::Helpers::EntryListProxy> proxyMock1(
			std::unique_ptr<EntryListProxyMock>(new EntryListProxyMock()));
		auto expected = proxyMock1.get();
		EntryListProxyProxy proxy(std::move(proxyMock1));
		ASSERT_EQ(proxyMock1, nullptr);
		proxy.swap(proxyMock1);
		EXPECT_EQ(proxyMock1.get(), expected);
	}
} // namespace Test::ZipDirFs::Components
