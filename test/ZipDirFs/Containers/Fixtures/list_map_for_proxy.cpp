/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "list_map_for_proxy.h"

namespace Test::ZipDirFs::Containers
{
	namespace Fixtures
	{
		list_map_for_proxy_create_helper* list_map_for_proxy_create_helper::instance;
		list_map_for_proxy_create_helper::list_map_for_proxy_create_helper() { instance = this; }
		list_map_for_proxy_create_helper::~list_map_for_proxy_create_helper()
		{
			instance = nullptr;
		}

		list_map_for_proxy::list_map_for_proxy(const compare_type& comp,
			const map_allocator& map_alloc, const list_allocator& list_alloc) :
			list_map_for_proxy_base(comp, map_alloc, list_alloc)
		{
			if (list_map_for_proxy_create_helper::instance != nullptr)
			{
				list_map_for_proxy_create_helper::instance->create(this);
			}
		}
		list_for_proxy<std::string>& list_map_for_proxy::get_list() { return _list; }
		map_mock<std::string, fusekit::entry*>& list_map_for_proxy::get_map() { return _map; }
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers
