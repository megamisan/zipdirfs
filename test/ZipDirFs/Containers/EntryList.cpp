/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */

#include "ZipDirFs/Containers/EntryList.h"
#include "EntryList.h"
#include "Fixtures/helpers.h"
#include "test/gtest.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Containers
{
	using Fixtures::list_for_proxy;
	using Fixtures::list_for_proxy_create_helper;
	using Fixtures::list_for_proxy_destruct_helper;
	using Fixtures::list_for_proxy_iterator_equals_helper;
	using Fixtures::list_map_for_proxy_create_helper;
	using Fixtures::map_mock;
	using ::testing::_;
	using ::testing::ByRef;
	using ::testing::Eq;
	using ::testing::Return;
	using ::testing::ReturnRef;

	_Link_type EntryListMockNodeContainer::getNode() { return _node; }
	EntryListMockNodeContainer::EntryListMockNodeContainer(_Val&& val)
	{
		_node = entryList_get_node();
		entryList_construct_node(_node, std::forward<_Val>(val));
	}
	EntryListMockNodeContainer::~EntryListMockNodeContainer() { delete _node; }
	_Link_type EntryListMockNodeContainer::entryList_get_node()
	{
		_Node_allocator alloc;
		return alloc.allocate(1);
	}
	void EntryListMockNodeContainer::entryList_construct_node(_Link_type __node, _Val&& arg)
	{
		_Node_allocator alloc;
		::new (__node) std::_Rb_tree_node<_Val>;
		alloc.construct(__node->_M_valptr(), std::forward<_Val>(arg));
	}

	list_map_mock& EntryListMockedAccess::list_map(EntryListMocked& elm)
	{
		return reinterpret_cast<EntryListMockedAccess*>(&elm)->container;
	}

	EntryListForProxyMocked::Proxy::iterator_proxy EntryListForProxyMockedAccess::getIteratorFor(
		const EntryListForProxyMocked::iterator& it)
	{
		return EntryIterator(new Wrapper(it));
	}
	::ZipDirFs::Containers::EntryIterator::Wrapper* EntryListForProxyMockedAccess::getWrapperFor(
		iterator&& it)
	{
		return new Wrapper(std::move(it));
	}
	::ZipDirFs::Containers::EntryIterator::Wrapper* EntryListForProxyMockedAccess::getWrapperFor(
		const iterator& it)
	{
		return new Wrapper(it);
	}

	EntryListForProxyMockedAccess::iterator& EntryListForProxyMockedAccess::WrapperAccess::getReal(
		EntryIterator::Wrapper& w)
	{
		return reinterpret_cast<WrapperAccess*>(&w)->_real;
	}
	EntryListForProxyMockedAccess::EntryIterator::Wrapper*
		EntryListForProxyMockedAccess::WrapperAccess::invokeClone(const EntryIterator::Wrapper& w)
	{
		return reinterpret_cast<const WrapperAccess*>(&w)->clone();
	}
	const std::string& EntryListForProxyMockedAccess::WrapperAccess::invokeDereference(
		EntryIterator::Wrapper& w)
	{
		return reinterpret_cast<WrapperAccess*>(&w)->dereference();
	}
	void EntryListForProxyMockedAccess::WrapperAccess::invokeIncrement(EntryIterator::Wrapper& w)
	{
		return reinterpret_cast<WrapperAccess*>(&w)->increment();
	}
	bool EntryListForProxyMockedAccess::WrapperAccess::invokeEquals(
		const EntryIterator::Wrapper& w1, const EntryIterator::Wrapper& w2)
	{
		return reinterpret_cast<const WrapperAccess*>(&w1)->equals(w2);
	}

	template <typename T>
	T* EntryListMockTest::generate()
	{
		return reinterpret_cast<T*>(::Test::rand(UINT32_MAX));
	}

	ACTION_P(Capture, output) { *output = arg0; }
	std::function<::testing::AssertionResult(const char*, const char*,
		const ::ZipDirFs::Containers::EntryIterator::Wrapper&,
		const ::ZipDirFs::Containers::EntryIterator::Wrapper&)>
		PredicateWrapperEquals(bool expected)
	{
		return [expected](const char* nameA, const char* nameB,
				   const ::ZipDirFs::Containers::EntryIterator::Wrapper& a,
				   const ::ZipDirFs::Containers::EntryIterator::Wrapper& b)
		{
			auto result = EntryListForProxyMockedAccess::WrapperAccess::invokeEquals(a, b);
			if (result == expected)
				return ::testing::AssertionSuccess();
			else
				return ::testing::internal::CmpHelperOpFailure(
					nameA, nameB, a, b, expected ? "==" : "!=");
		};
	}

	TEST_F(EntryListMockTest, begin)
	{
		EntryListMocked el;
		EntryListMocked::iterator iterator(generate<std::__detail::_List_node_base>());
		EXPECT_CALL(EntryListMockedAccess::list_map(el), begin()).WillOnce(Return(iterator));
		EXPECT_EQ(iterator, el.begin());
	}

	TEST_F(EntryListMockTest, end)
	{
		EntryListMocked el;
		EntryListMocked::iterator iterator(generate<std::__detail::_List_node_base>());
		EXPECT_CALL(EntryListMockedAccess::list_map(el), end()).WillOnce(Return(iterator));
		EXPECT_EQ(iterator, el.end());
	}

	TEST_F(EntryListMockTest, insert)
	{
		EntryListMocked el;
		list_map_mock::list_type::iterator origin(generate<std::__detail::_List_node_base>());
#if __cplusplus >= 201103L
		list_map_mock::list_type::const_iterator position(origin);
#else
		list_map_mock::list_type::iterator position(origin);
#endif
		EntryListMocked::iterator iterator(generate<std::__detail::_List_node_base>());
		const std::string name("name");
		fusekit::entry* entry(generate<fusekit::entry>());
		EXPECT_CALL(EntryListMockedAccess::list_map(el), insert(position, name, entry))
			.WillOnce(Return(iterator));
		EXPECT_EQ(iterator, el.insert(origin, name, entry));
	}

	TEST_F(EntryListMockTest, erase)
	{
		EntryListMocked el;
		list_map_mock::list_type::iterator origin(generate<std::__detail::_List_node_base>());
#if __cplusplus >= 201103L
		list_map_mock::list_type::const_iterator position(origin);
#else
		list_map_mock::list_type::iterator position(origin);
#endif
		EntryListMocked::iterator iterator(generate<std::__detail::_List_node_base>());
		EXPECT_CALL(EntryListMockedAccess::list_map(el), erase(position))
			.WillOnce(Return(iterator));
		EXPECT_EQ(iterator, el.erase(origin));
	}

	TEST_F(EntryListMockTest, find_absent)
	{
		::testing::InSequence seq;
		EntryListMocked el;
		std::string name("name");
		list_map_mock::map_type::iterator iterator(generate<std::_Rb_tree_node_base>());
		EXPECT_CALL(EntryListMockedAccess::list_map(el), find(name)).WillOnce(Return(iterator));
		EXPECT_CALL(EntryListMockedAccess::list_map(el), valid(iterator))
			.WillOnce(Return<fusekit::entry*>(nullptr));
		EXPECT_EQ(nullptr, el.find(name));
	}

	TEST_F(EntryListMockTest, find_exists)
	{
		::testing::InSequence seq;
		EntryListMocked el;
		std::string name("name");
		fusekit::entry* entry(generate<fusekit::entry>());
		EntryListMockNodeContainer nodeContainer({name, entry});
		list_map_mock::map_type::iterator iterator(nodeContainer.getNode());
		EXPECT_CALL(EntryListMockedAccess::list_map(el), find(name)).WillOnce(Return(iterator));
		EXPECT_CALL(EntryListMockedAccess::list_map(el), valid(iterator)).WillOnce(Return(true));
		EXPECT_EQ(entry, el.find(name));
	}

	TEST(EntryListWrapperTest, createMove)
	{
		list_for_proxy<std::string> expectedList{std::allocator<std::string>()};
		size_t expectedPosition = ::Test::rand(UINT32_MAX);
		::ZipDirFs::Containers::EntryIterator::pointer_wrapper result(
			EntryListForProxyMockedAccess::getWrapperFor(
				list_for_proxy<std::string>::iterator(&expectedList, expectedPosition)));
		list_for_proxy<std::string>::iterator& resultIt =
			EntryListForProxyMockedAccess::WrapperAccess::getReal(*result);
		EXPECT_EQ(&expectedList, resultIt.getSource());
		EXPECT_EQ(expectedPosition, resultIt.getPosition());
	}
	TEST(EntryListWrapperTest, createCopy)
	{
		list_for_proxy<std::string> expectedList{std::allocator<std::string>()};
		list_for_proxy<std::string>::iterator expectedIt(&expectedList, ::Test::rand(UINT32_MAX));
		::ZipDirFs::Containers::EntryIterator::pointer_wrapper result(
			EntryListForProxyMockedAccess::getWrapperFor(expectedIt));
		list_for_proxy<std::string>::iterator& resultIt =
			EntryListForProxyMockedAccess::WrapperAccess::getReal(*result);
		EXPECT_NE(&expectedIt, &resultIt);
		EXPECT_EQ(expectedIt.getSource(), resultIt.getSource());
		EXPECT_EQ(expectedIt.getPosition(), resultIt.getPosition());
	}
	TEST(EntryListWrapperTest, clone)
	{
		list_for_proxy<std::string> expectedList{std::allocator<std::string>()};
		list_for_proxy<std::string>::iterator expectedIt(&expectedList, ::Test::rand(UINT32_MAX));
		::ZipDirFs::Containers::EntryIterator::pointer_wrapper wrapper(
			EntryListForProxyMockedAccess::getWrapperFor(expectedIt));
		::ZipDirFs::Containers::EntryIterator::pointer_wrapper result(
			EntryListForProxyMockedAccess::WrapperAccess::invokeClone(*wrapper));
		list_for_proxy<std::string>::iterator& resultIt =
			EntryListForProxyMockedAccess::WrapperAccess::getReal(*result);
		EXPECT_NE(&expectedIt, &resultIt);
		EXPECT_EQ(expectedIt.getSource(), resultIt.getSource());
		EXPECT_EQ(expectedIt.getPosition(), resultIt.getPosition());
	}
	TEST(EntryListWrapperTest, dereference)
	{
		list_for_proxy<std::string> expectedList{std::allocator<std::string>()};
		list_for_proxy<std::string>::iterator expectedIt(&expectedList, ::Test::rand(UINT32_MAX));
		::ZipDirFs::Containers::EntryIterator::pointer_wrapper wrapper(
			EntryListForProxyMockedAccess::getWrapperFor(expectedIt));
		std::string expected;
		EXPECT_CALL(expectedList, getValue(expectedIt.getPosition())).WillOnce(ReturnRef(expected));
		const std::string& result =
			EntryListForProxyMockedAccess::WrapperAccess::invokeDereference(*wrapper);
		EXPECT_EQ(&expected, &result);
	}
	TEST(EntryListWrapperTest, increment)
	{
		list_for_proxy<std::string> expectedList{std::allocator<std::string>()};
		list_for_proxy<std::string>::iterator expectedIt(&expectedList, ::Test::rand(UINT32_MAX));
		::ZipDirFs::Containers::EntryIterator::pointer_wrapper wrapper(
			EntryListForProxyMockedAccess::getWrapperFor(expectedIt));
		EXPECT_CALL(expectedList, size()).WillOnce(Return(::Test::rand(UINT32_MAX)));
		EntryListForProxyMockedAccess::WrapperAccess::invokeIncrement(*wrapper);
		const size_t result =
			EntryListForProxyMockedAccess::WrapperAccess::getReal(*wrapper).getPosition();
		EXPECT_THAT(
			result, ::testing::AnyOf(::testing::Gt(expectedIt.getPosition()), Eq(UINT64_MAX)));
	}
	TEST(EntryListWrapperTest, equalsTrue)
	{
		list_for_proxy<std::string> expectedList{std::allocator<std::string>()};
		size_t expectedPosition = ::Test::rand(UINT32_MAX);
		list_for_proxy<std::string>::iterator expectedIt1(&expectedList, expectedPosition);
		::ZipDirFs::Containers::EntryIterator::pointer_wrapper wrapper1(
			EntryListForProxyMockedAccess::getWrapperFor(expectedIt1));
		list_for_proxy<std::string>::iterator expectedIt2(&expectedList, expectedPosition);
		::ZipDirFs::Containers::EntryIterator::pointer_wrapper wrapper2(
			EntryListForProxyMockedAccess::getWrapperFor(expectedIt2));
		list_for_proxy_iterator_equals_helper h;
		EXPECT_CALL(h, equals());
		EXPECT_PRED_FORMAT2(PredicateWrapperEquals(true), *wrapper1, *wrapper2);
	}
	TEST(EntryListWrapperTest, equalsFalsePosition)
	{
		list_for_proxy<std::string> expectedList{std::allocator<std::string>()};
		list_for_proxy<std::string>::iterator expectedIt1(&expectedList, ::Test::rand(UINT32_MAX));
		::ZipDirFs::Containers::EntryIterator::pointer_wrapper wrapper1(
			EntryListForProxyMockedAccess::getWrapperFor(expectedIt1));
		list_for_proxy<std::string>::iterator expectedIt2(&expectedList, ::Test::rand(UINT32_MAX));
		::ZipDirFs::Containers::EntryIterator::pointer_wrapper wrapper2(
			EntryListForProxyMockedAccess::getWrapperFor(expectedIt2));
		list_for_proxy_iterator_equals_helper h;
		EXPECT_CALL(h, equals());
		EXPECT_PRED_FORMAT2(PredicateWrapperEquals(false), *wrapper1, *wrapper2);
	}
	TEST(EntryListWrapperTest, equalsFalseList)
	{
		list_for_proxy<std::string> expectedList1{std::allocator<std::string>()};
		list_for_proxy<std::string> expectedList2{std::allocator<std::string>()};
		list_for_proxy<std::string>::iterator expectedIt1(&expectedList1, ::Test::rand(UINT32_MAX));
		::ZipDirFs::Containers::EntryIterator::pointer_wrapper wrapper1(
			EntryListForProxyMockedAccess::getWrapperFor(expectedIt1));
		list_for_proxy<std::string>::iterator expectedIt2(&expectedList2, ::Test::rand(UINT32_MAX));
		::ZipDirFs::Containers::EntryIterator::pointer_wrapper wrapper2(
			EntryListForProxyMockedAccess::getWrapperFor(expectedIt2));
		list_for_proxy_iterator_equals_helper h;
		EXPECT_CALL(h, equals());
		EXPECT_PRED_FORMAT2(PredicateWrapperEquals(false), *wrapper1, *wrapper2);
	}
	TEST(EntryListWrapperTest, equalsFalseType)
	{
		list_for_proxy<std::string> expectedList{std::allocator<std::string>()};
		size_t expectedPosition = ::Test::rand(UINT32_MAX);
		list_for_proxy<std::string>::iterator expectedIt1(&expectedList, expectedPosition);
		::ZipDirFs::Containers::EntryIterator::pointer_wrapper wrapper1(
			EntryListForProxyMockedAccess::getWrapperFor(expectedIt1));
		::ZipDirFs::Containers::EntryIterator::pointer_wrapper wrapper2(new WrapperNull());
		list_for_proxy_iterator_equals_helper h;
		EXPECT_PRED_FORMAT2(PredicateWrapperEquals(false), *wrapper1, *wrapper2);
	}

	TEST(EntryListProxyTest, instantiate)
	{
		list_for_proxy_create_helper<list_map_for_proxy::list_allocator> c;
		EXPECT_CALL(c, create(_));
		std::shared_ptr<EntryListProxy> list_map_for_proxy(
			EntryListForProxyMockedAccess::createWithProxy());
		EXPECT_NE(nullptr, list_map_for_proxy.get());
	}

	TEST(EntryListProxyTest, destruct)
	{
		list_map_for_proxy_create_helper c;
		list_for_proxy_destruct_helper<std::string> d;
		list_map_for_proxy* created;
		EXPECT_CALL(c, create(_)).WillOnce(Capture(&created));
		std::shared_ptr<EntryListProxy> entryListProxy(
			EntryListForProxyMockedAccess::createWithProxy());
		EXPECT_CALL(d, destruct(&created->get_list()));
		entryListProxy = nullptr;
	}

	TEST(EntryListProxyTest, begin)
	{
		list_map_for_proxy_create_helper c;
		list_map_for_proxy* created;
		EXPECT_CALL(c, create(_)).WillOnce(Capture(&created));
		std::shared_ptr<EntryListProxy> entryListProxy(
			EntryListForProxyMockedAccess::createWithProxy());
		list_for_proxy<std::string>::iterator expectedBase(&created->get_list(), 0);
		EXPECT_CALL(created->get_list(), begin()).WillOnce(Return(expectedBase));
		::ZipDirFs::Containers::EntryIterator expected =
			EntryListForProxyMockedAccess::getIteratorFor(expectedBase);
		::ZipDirFs::Containers::EntryIterator result = entryListProxy->begin();
		EXPECT_EQ(expected, result);
	}

	TEST(EntryListProxyTest, end)
	{
		list_map_for_proxy_create_helper c;
		list_map_for_proxy* created;
		EXPECT_CALL(c, create(_)).WillOnce(Capture(&created));
		std::shared_ptr<EntryListProxy> entryListProxy(
			EntryListForProxyMockedAccess::createWithProxy());
		list_for_proxy<std::string>::iterator expectedBase(&created->get_list(), -1);
		EXPECT_CALL(created->get_list(), end()).WillOnce(Return(expectedBase));
		::ZipDirFs::Containers::EntryIterator expected =
			EntryListForProxyMockedAccess::getIteratorFor(expectedBase);
		::ZipDirFs::Containers::EntryIterator result = entryListProxy->end();
		EXPECT_EQ(expected, result);
	}

	TEST(EntryListProxyTest, insert)
	{
		list_map_for_proxy_create_helper c;
		list_map_for_proxy* created;
		EXPECT_CALL(c, create(_)).WillOnce(Capture(&created));
		std::shared_ptr<EntryListProxy> entryListProxy(
			EntryListForProxyMockedAccess::createWithProxy());
		list_for_proxy<std::string>::iterator insertBase(
			&created->get_list(), ::Test::rand(UINT32_MAX));
		list_for_proxy<std::string>::iterator expectedBase(
			&created->get_list(), ::Test::rand(UINT32_MAX));
		EXPECT_CALL(created->get_map(), insert(_));
		EXPECT_CALL(created->get_list(), insert(insertBase, "")).WillOnce(Return(expectedBase));
		::ZipDirFs::Containers::EntryIterator insert =
			EntryListForProxyMockedAccess::getIteratorFor(insertBase);
		::ZipDirFs::Containers::EntryIterator expected =
			EntryListForProxyMockedAccess::getIteratorFor(expectedBase);
		fusekit::entry* ref = nullptr;
		::ZipDirFs::Containers::EntryIterator result = entryListProxy->insert(insert, "", ref);
		EXPECT_EQ(expected, result);
	}

	TEST(EntryListProxyTest, erase)
	{
		list_map_for_proxy_create_helper c;
		list_map_for_proxy* created;
		EXPECT_CALL(c, create(_)).WillOnce(Capture(&created));
		std::shared_ptr<EntryListProxy> entryListProxy(
			EntryListForProxyMockedAccess::createWithProxy());
		list_for_proxy<std::string>::iterator eraseBase(
			&created->get_list(), ::Test::rand(UINT32_MAX));
		list_for_proxy<std::string>::iterator expectedBase(
			&created->get_list(), ::Test::rand(UINT32_MAX));
		const std::string value("");
		EXPECT_CALL(created->get_list(), getValue(eraseBase.getPosition()))
			.WillOnce(ReturnRef(value));
		EXPECT_CALL(created->get_map(), erase(Eq(ByRef(value))));
		EXPECT_CALL(created->get_list(), erase(eraseBase)).WillOnce(Return(expectedBase));
		::ZipDirFs::Containers::EntryIterator erase =
			EntryListForProxyMockedAccess::getIteratorFor(eraseBase);
		::ZipDirFs::Containers::EntryIterator expected =
			EntryListForProxyMockedAccess::getIteratorFor(expectedBase);
		::ZipDirFs::Containers::EntryIterator result = entryListProxy->erase(erase);
		EXPECT_EQ(expected, result);
	}

	TEST(EntryListProxyTest, find)
	{
		list_map_for_proxy_create_helper c;
		list_map_for_proxy* created;
		EXPECT_CALL(c, create(_)).WillOnce(Capture(&created));
		std::shared_ptr<EntryListProxy> entryListProxy(
			EntryListForProxyMockedAccess::createWithProxy());
		list_map_for_proxy::map_type::const_iterator expectedBase;
		EXPECT_CALL(const_cast<const list_map_for_proxy::map_type&>(created->get_map()), find(""))
			.WillOnce(Return(expectedBase));
		EXPECT_CALL(const_cast<const list_map_for_proxy::map_type&>(created->get_map()), end())
			.WillOnce(Return(expectedBase));
		fusekit::entry* result = entryListProxy->find("");
		EXPECT_EQ(nullptr, result);
	}
} // namespace Test::ZipDirFs::Containers
