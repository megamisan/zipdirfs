/*
 * Copyright © 2019-2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Containers/list_map.h"
#include "Fixtures/helpers.h"
#include "list_map.h"
#include "test/gtest.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

namespace Test::ZipDirFs::Containers
{
	using ::testing::_;
	using ::testing::ByRef;
	using ::testing::Eq;
	using ::testing::Return;
	using ::testing::ReturnRef;

	list_map_mocked::list_type& list_map_mocked_access::list(list_map_mocked& lm)
	{
		return reinterpret_cast<list_map_mocked_access*>(&lm)->_list;
	}
	list_map_mocked::map_type& list_map_mocked_access::map(list_map_mocked& lm)
	{
		return reinterpret_cast<list_map_mocked_access*>(&lm)->_map;
	}
	const list_map_mocked::list_type& list_map_mocked_access::list(const list_map_mocked& lm)
	{
		return reinterpret_cast<const list_map_mocked_access*>(&lm)->_list;
	}
	const list_map_mocked::map_type& list_map_mocked_access::map(const list_map_mocked& lm)
	{
		return reinterpret_cast<const list_map_mocked_access*>(&lm)->_map;
	}

	list_map_test::list_map_test() :
		::testing::Test(), first_value(::Test::rand(UINT32_MAX)), second_value(::Test::rand(UINT32_MAX)),
		first_key(generateKey()), second_key(generateKey())
	{
	}
	void list_map_test::SetUp()
	{
		first_iterator = lm.insert(lm.end(), first_key, first_value);
		second_iterator = lm.insert(lm.end(), second_key, second_value);
	}
	const std::string list_map_test::generateKey()
	{
		const char min = 'a', max = 'z';
		const int diviser = (RAND_MAX + 1u) / (max - min + 1);
		const std::uint8_t length = ::Test::rand(min, max);
		std::uint8_t position = 0;
		std::string result;
		while (position < length)
		{
			result += (char)(::Test::rand(min, max));
			position++;
		}
		return result;
	}

	TEST_F(list_map_mock_test, instantiate)
	{
		list_map_mocked* object = nullptr;
		list_mock_helper_type lmh;
		map_mock_helper_type mmh;
		EXPECT_CALL(lmh, create(&list_allocator));
		EXPECT_CALL(mmh, create(&compare, &map_allocator));
		EXPECT_NO_THROW(object = new list_map_mocked(compare, map_allocator, list_allocator))
			<< "Constructor failed";
		if (object != nullptr)
		{
			delete object;
		}
	}

	TEST_F(list_map_mock_test, allocators)
	{
		list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::map(lm), get_allocator());
		EXPECT_CALL(list_map_mocked_access::list(lm), get_allocator());
		EXPECT_EQ(map_allocator, lm.get_map_allocator()) << "Map allocator does not match";
		EXPECT_EQ(list_allocator, lm.get_list_allocator()) << "List allocator does not match";
	}

	TEST_F(list_map_mock_test, begin)
	{
		list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::list(lm), begin()).WillOnce(Return(list_type::iterator()));
		lm.begin();
	}

	TEST_F(list_map_mock_test, begin_const)
	{
		const list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::list(lm), begin()).WillOnce(Return(list_type::const_iterator()));
		lm.begin();
	}

	TEST_F(list_map_mock_test, end)
	{
		list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::list(lm), end()).WillOnce(Return(list_type::iterator()));
		lm.end();
	}

	TEST_F(list_map_mock_test, end_const)
	{
		const list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::list(lm), end()).WillOnce(Return(list_type::const_iterator()));
		lm.end();
	}

	TEST_F(list_map_mock_test, rbegin)
	{
		list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::list(lm), rbegin()).WillOnce(Return(list_type::reverse_iterator()));
		lm.rbegin();
	}

	TEST_F(list_map_mock_test, rbegin_const)
	{
		const list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::list(lm), rbegin()).WillOnce(Return(list_type::const_reverse_iterator()));
		lm.rbegin();
	}

	TEST_F(list_map_mock_test, rend)
	{
		list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::list(lm), rend()).WillOnce(Return(list_type::reverse_iterator()));
		lm.rend();
	}

	TEST_F(list_map_mock_test, rend_const)
	{
		const list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::list(lm), rend()).WillOnce(Return(list_type::const_reverse_iterator()));
		lm.rend();
	}

#if __cplusplus >= 201103L
	TEST_F(list_map_mock_test, cbegin)
	{
		const list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::list(lm), cbegin()).WillOnce(Return(list_type::const_iterator()));
		lm.cbegin();
	}

	TEST_F(list_map_mock_test, cend)
	{
		const list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::list(lm), cend()).WillOnce(Return(list_type::const_iterator()));
		lm.cend();
	}

	TEST_F(list_map_mock_test, crbegin)
	{
		const list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::list(lm), crbegin()).WillOnce(Return(list_type::const_reverse_iterator()));
		lm.crbegin();
	}

	TEST_F(list_map_mock_test, crend)
	{
		const list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::list(lm), crend()).WillOnce(Return(list_type::const_reverse_iterator()));
		lm.crend();
	}
#endif

	TEST_F(list_map_mock_test, size)
	{
		const list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::list(lm), size()).WillOnce(Return(0));
		lm.size();
	}

	TEST_F(list_map_mock_test, empty)
	{
		const list_map_mocked lm(compare, map_allocator, list_allocator);
		EXPECT_CALL(list_map_mocked_access::list(lm), empty()).WillOnce(Return(true));
		lm.empty();
	}

	TEST_F(list_map_mock_test, front)
	{
		list_map_mocked lm(compare, map_allocator, list_allocator);
		std::string key("front");
		EXPECT_CALL(list_map_mocked_access::list(lm), front()).WillOnce(ReturnRef(key));
		lm.front();
	}

	TEST_F(list_map_mock_test, front_const)
	{
		const list_map_mocked lm(compare, map_allocator, list_allocator);
		const std::string key("front_const");
		EXPECT_CALL(list_map_mocked_access::list(lm), front()).WillOnce(ReturnRef(key));
		lm.front();
	}

	TEST_F(list_map_mock_test, back)
	{
		list_map_mocked lm(compare, map_allocator, list_allocator);
		std::string key("back");
		EXPECT_CALL(list_map_mocked_access::list(lm), back()).WillOnce(ReturnRef(key));
		lm.back();
	}

	TEST_F(list_map_mock_test, back_const)
	{
		const list_map_mocked lm(compare, map_allocator, list_allocator);
		const std::string key("back_const");
		EXPECT_CALL(list_map_mocked_access::list(lm), back()).WillOnce(ReturnRef(key));
		lm.back();
	}

	TEST_F(list_map_mock_test, insert)
	{
		list_map_mocked lm(compare, map_allocator, list_allocator);
#if __cplusplus >= 201103L
		list_type::const_iterator position;
#else
		list_type::iterator position;
#endif
		std::string key("key");
		std::int32_t value(INT32_MAX);
		::testing::InSequence seq;
		EXPECT_CALL(list_map_mocked_access::map(lm), insert(std::pair<const std::string, std::int32_t>(key, value)))
			.WillOnce(Return(std::make_pair<map_type::iterator, bool>(map_type::iterator(), true)));
		EXPECT_CALL(list_map_mocked_access::list(lm), insert(position, Eq(ByRef(key))))
			.WillOnce(Return(list_type::iterator()));
		lm.insert(position, key, value);
	}

	TEST_F(list_map_mock_test, erase)
	{
		list_map_mocked lm(compare, map_allocator, list_allocator);
		std::_List_node<std::string> node;
		new (node._M_valptr()) std::string("key");
		std::unique_ptr<std::string, std::function<void(std::string*)>> temporaryNode(
			node._M_valptr(), [](std::string* s) { s->~basic_string(); });
		list_type::const_iterator position(&node);
		::testing::InSequence seq;
		EXPECT_CALL(list_map_mocked_access::map(lm), erase(Eq(ByRef(*position)))).WillOnce(Return(1));
		EXPECT_CALL(list_map_mocked_access::list(lm), erase(position)).WillOnce(Return(list_type::iterator()));
		lm.erase(position);
	}

	TEST_F(list_map_mock_test, find)
	{
		list_map_mocked lm(compare, map_allocator, list_allocator);
		std::_Rb_tree_node<map_type::value_type> node;
		map_type::iterator result(&node);
		const std::string name("key");
		EXPECT_CALL(list_map_mocked_access::map(lm), find(Eq(ByRef(name)))).WillOnce(Return(result));
		lm.find(name);
	}

	TEST_F(list_map_mock_test, find_const)
	{
		const list_map_mocked lm(compare, map_allocator, list_allocator);
		std::_Rb_tree_node<map_type::value_type> node;
		map_type::const_iterator result(&node);
		const std::string name("key");
		EXPECT_CALL(list_map_mocked_access::map(lm), find(Eq(ByRef(name)))).WillOnce(Return(result));
		lm.find(name);
	}

	TEST_F(list_map_mock_test, valid)
	{
		list_map_mocked lm(compare, map_allocator, list_allocator);
		std::_Rb_tree_node<map_type::value_type> node;
		map_type::iterator result(&node);
		EXPECT_CALL(list_map_mocked_access::map(lm), end()).WillOnce(Return(map_type::iterator()));
		EXPECT_TRUE(lm.valid(result));
	}

	TEST_F(list_map_mock_test, valid_const)
	{
		const list_map_mocked lm(compare, map_allocator, list_allocator);
		std::_Rb_tree_node<map_type::value_type> node;
		map_type::const_iterator result(&node);
		EXPECT_CALL(list_map_mocked_access::map(lm), end()).WillOnce(Return(map_type::const_iterator()));
		EXPECT_TRUE(lm.valid(result));
	}

	TEST_F(list_map_test, value_ok)
	{
		list_map::map_type::iterator itFirst = lm.find(first_key);
		list_map::map_type::iterator itSecond = lm.find(second_key);
		ASSERT_TRUE(lm.valid(itFirst));
		ASSERT_TRUE(lm.valid(itSecond));
		EXPECT_EQ(first_value, itFirst->second);
		EXPECT_EQ(second_value, itSecond->second);
	}

	TEST_F(list_map_test, value_ok_const)
	{
		const list_map& clm = lm;
		list_map::map_type::const_iterator itFirst = clm.find(first_key);
		list_map::map_type::const_iterator itSecond = clm.find(second_key);
		ASSERT_TRUE(clm.valid(itFirst));
		ASSERT_TRUE(clm.valid(itSecond));
		EXPECT_EQ(first_value, itFirst->second);
		EXPECT_EQ(second_value, itSecond->second);
	}

	TEST_F(list_map_test, insert_first)
	{
		const std::string new_key(generateKey());
		const std::int32_t new_value(::Test::rand(UINT32_MAX));
		lm.insert(first_iterator, new_key, new_value);
		list_map::list_type::iterator iterator = lm.begin();
		EXPECT_EQ(new_key, *(iterator++));
		EXPECT_EQ(first_key, *(iterator++));
		EXPECT_EQ(second_key, *(iterator++));
	}

	TEST_F(list_map_test, insert_second)
	{
		const std::string new_key(generateKey());
		const std::int32_t new_value(::Test::rand(UINT32_MAX));
		lm.insert(second_iterator, new_key, new_value);
		list_map::list_type::iterator iterator = lm.begin();
		EXPECT_EQ(first_key, *(iterator++));
		EXPECT_EQ(new_key, *(iterator++));
		EXPECT_EQ(second_key, *(iterator++));
	}

	TEST_F(list_map_test, insert_last)
	{
		const std::string new_key(generateKey());
		const std::int32_t new_value(::Test::rand(UINT32_MAX));
		lm.insert(lm.end(), new_key, new_value);
		list_map::list_type::iterator iterator = lm.begin();
		EXPECT_EQ(first_key, *(iterator++));
		EXPECT_EQ(second_key, *(iterator++));
		EXPECT_EQ(new_key, *(iterator++));
	}

	TEST_F(list_map_test, value_nok)
	{
		const std::string non_existant(generateKey());
		list_map::map_type::iterator it = lm.find(non_existant);
		EXPECT_FALSE(lm.valid(it));
	}

	TEST_F(list_map_test, value_nok_const)
	{
		const list_map& clm = lm;
		const std::string non_existant(generateKey());
		list_map::map_type::const_iterator it = clm.find(non_existant);
		EXPECT_FALSE(lm.valid(it));
	}
} // namespace Test::ZipDirFs::Containers
