/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "list_cleanup.h"
#include "ZipDirFs/Utilities/list_cleanup.h"
#include "test/gtest.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Utilities
{
	using ::testing::Return;

	namespace
	{
		struct FactoryMock : public ::ZipDirFs::Containers::Helpers::Factory
		{
			MOCK_METHOD1(create, const mapped_type(const key_type&));
			MOCK_METHOD1(destroy, void(const mapped_type&));
		};

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

	TEST(list_cleanupTest, Execute)
	{
		::testing::InSequence is;
		EntryListProxyMock elp;
		FactoryMock f;
		std::string name1("entry" + std::to_string(::Test::rand(UINT32_MAX)));
		std::string name2("entry" + std::to_string(::Test::rand(UINT32_MAX)));
		std::string end("end");
		fusekit::entry* entry1 = reinterpret_cast<fusekit::entry*>(::Test::rand(UINT32_MAX));
		fusekit::entry* entry2 = reinterpret_cast<fusekit::entry*>(::Test::rand(UINT32_MAX));
		EXPECT_CALL(elp, begin())
			.WillOnce(
				Return(::ZipDirFs::Containers::EntryIterator(new EntryIteratorWrapperTest(name1))));
		EXPECT_CALL(elp, end())
			.WillOnce(
				Return(::ZipDirFs::Containers::EntryIterator(new EntryIteratorWrapperTest(end))));
		EXPECT_CALL(elp, find(name1)).WillOnce(Return(entry1));
		EXPECT_CALL(f, destroy(entry1));
		EXPECT_CALL(
			elp, erase(::ZipDirFs::Containers::EntryIterator(new EntryIteratorWrapperTest(name1))))
			.WillOnce(
				Return(::ZipDirFs::Containers::EntryIterator(new EntryIteratorWrapperTest(name2))));
		EXPECT_CALL(elp, find(name2)).WillOnce(Return(entry2));
		EXPECT_CALL(f, destroy(entry2));
		EXPECT_CALL(
			elp, erase(::ZipDirFs::Containers::EntryIterator(new EntryIteratorWrapperTest(name2))))
			.WillOnce(
				Return(::ZipDirFs::Containers::EntryIterator(new EntryIteratorWrapperTest(end))));
		::ZipDirFs::Utilities::CleanupEntryList(elp, f);
	}
} // namespace Test::ZipDirFs::Utilities
