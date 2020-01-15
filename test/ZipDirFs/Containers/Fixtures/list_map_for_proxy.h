/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_FIXTURES_LIST_MAP_FOR_PROXY_H_INCLUDED
#define TEST_ZIPDIRFS_CONTAINERS_FIXTURES_LIST_MAP_FOR_PROXY_H_INCLUDED

#include "ZipDirFs/Containers/list_map.h"
#include "list_for_proxy.h"
#include "map_mock.h"
#include <gmock/gmock.h>

namespace fusekit
{
	struct entry;
}

namespace Test::ZipDirFs::Containers
{
	namespace Fixtures
	{
		struct list_map_for_proxy;

		struct list_map_for_proxy_create_helper
		{
			MOCK_METHOD1_T(create, void(list_map_for_proxy* instance));
			static list_map_for_proxy_create_helper* instance;
			list_map_for_proxy_create_helper();
			~list_map_for_proxy_create_helper();
		};

		using list_map_for_proxy_base =
			::ZipDirFs::Containers::list_map<std::string, fusekit::entry*, std::less<std::string>,
				std::allocator<std::pair<const std::string, fusekit::entry*>>,
				std::allocator<std::string>, list_for_proxy<std::string>,
				map_mock<std::string, fusekit::entry*>>;
		struct list_map_for_proxy : public list_map_for_proxy_base
		{
			list_map_for_proxy(const compare_type& comp = compare_type(),
				const map_allocator& map_alloc = map_allocator(),
				const list_allocator& list_alloc = list_allocator());
			list_for_proxy<std::string>& get_list();
			map_mock<std::string, fusekit::entry*>& get_map();
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers

#endif // TEST_ZIPDIRFS_CONTAINERS_FIXTURES_LIST_MAP_FOR_PROXY_H_INCLUDED
