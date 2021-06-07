/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_ENTRYLIST_H_INCLUDED
#define TEST_ZIPDIRFS_CONTAINERS_ENTRYLIST_H_INCLUDED

#include "Fixtures/list_map_for_proxy.h"
#include "Fixtures/list_map_mock.h"
#include "ZipDirFs/Containers/EntryList.h"
#include <gmock/gmock.h>

namespace Test::ZipDirFs::Containers
{
	using Fixtures::list_map_for_proxy;
	using Fixtures::list_map_mock;
	using _Val = std::pair<const std::string, fusekit::entry*>;
	using _Alloc = std::allocator<_Val>;
	using EntryListMocked = ::ZipDirFs::Containers::EntryList<std::less<std::string>, _Alloc,
		std::allocator<std::string>, list_map_mock>;
	using _Node_allocator =
		__gnu_cxx::__alloc_traits<_Alloc>::template rebind<std::_Rb_tree_node<_Val>>::other;
	using _Alloc_traits = __gnu_cxx::__alloc_traits<_Node_allocator>;
	using _Link_type = std::_Rb_tree_node<_Val>*;
	using ::ZipDirFs::Containers::Helpers::EntryListProxy;
	using EntryListForProxyMocked = ::ZipDirFs::Containers::EntryList<std::less<std::string>,
		_Alloc, std::allocator<std::string>, list_map_for_proxy>;
	using WrapperBase = ::ZipDirFs::Containers::EntryIterator::Wrapper;

	struct EntryListMockNodeContainer
	{
		_Link_type getNode();
		EntryListMockNodeContainer(_Val&&);
		~EntryListMockNodeContainer();

	protected:
		_Link_type entryList_get_node();
		void entryList_construct_node(_Link_type, _Val&&);
		_Link_type _node;
	};

	struct EntryListMockedAccess : public EntryListMocked
	{
		static list_map_mock& list_map(EntryListMocked&);
	};

	struct EntryListForProxyMockedAccess : public EntryListForProxyMocked
	{
		using EntryIterator = ::ZipDirFs::Containers::EntryIterator;
		static Proxy::iterator_proxy getIteratorFor(const EntryListForProxyMocked::iterator&);
		struct WrapperAccess : Wrapper
		{
			static iterator& getReal(EntryIterator::Wrapper&);
			static EntryIterator::Wrapper* invokeClone(const EntryIterator::Wrapper&);
			static const std::string& invokeDereference(EntryIterator::Wrapper&);
			static void invokeIncrement(EntryIterator::Wrapper&);
			static bool invokeEquals(const EntryIterator::Wrapper&, const EntryIterator::Wrapper&);
		};
		static ::ZipDirFs::Containers::EntryIterator::Wrapper* getWrapperFor(iterator&&);
		static ::ZipDirFs::Containers::EntryIterator::Wrapper* getWrapperFor(const iterator&);
	};

	struct WrapperNull : public WrapperBase
	{
		~WrapperNull() {}

	protected:
		WrapperBase* clone() const { return nullptr; };
		reference dereference() const { throw nullptr; };
		void increment(){};
		bool equals(const WrapperBase&) const { return false; };
	};

	class EntryListMockTest : public ::testing::Test
	{
	protected:
		template <typename T>
		T* generate();
	};
} // namespace Test::ZipDirFs::Containers

#endif // TEST_ZIPDIRFS_CONTAINERS_ENTRYLIST_H_INCLUDED
