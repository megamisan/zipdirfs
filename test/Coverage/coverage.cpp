/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "../ZipDirFs/Containers/Fixtures/EntryIteratorWrapperMock.h"
#include "../ZipDirFs/Containers/Fixtures/helpers.h"
#include "../ZipDirFs/Containers/Fixtures/list_for_proxy.h"
#include "../ZipDirFs/Containers/Fixtures/list_map_for_proxy.h"
#include "../ZipDirFs/Containers/Fixtures/list_map_mock.h"
#include "../ZipDirFs/Containers/Fixtures/list_mock.h"
#include "../ZipDirFs/Containers/Fixtures/map_mock.h"
#include "ZipDirFs/Containers/EntryList.h"

namespace
{
	using Test::ZipDirFs::Containers::Fixtures::list_map_for_proxy;
	using Test::ZipDirFs::Containers::Fixtures::list_map_mock;
	using Test::ZipDirFs::Containers::Fixtures::list_mock;
	using Test::ZipDirFs::Containers::Fixtures::list_mock_helper;
	using Test::ZipDirFs::Containers::Fixtures::map_mock;
	using Test::ZipDirFs::Containers::Fixtures::map_mock_helper;
	using list_map__list_type = list_mock<std::string>;
	using list_map__map_type = map_mock<std::string, std::int32_t>;
	using list_map__compare_type = list_map__map_type::key_compare;
	using list_map__list_allocator_type = list_map__list_type::allocator_type;
	using list_map__map_allocator_type = list_map__map_type::allocator_type;
	using EntryList___Val = std::pair<const std::string, fusekit::entry*>;
	using EntryList___Alloc = std::allocator<EntryList___Val>;

	struct CoverageHelper
	{
		using list_map__list_map_mocked_base_type = ::ZipDirFs::Containers::list_map<std::string,
			std::int32_t, list_map__compare_type, list_map__map_allocator_type,
			list_map__list_allocator_type, list_map__list_type, list_map__map_type>;
		using list_map__list_mock_helper_type = list_mock_helper<list_map__list_allocator_type>;
		using list_map__map_mock_helper_type =
			map_mock_helper<list_map__compare_type, list_map__map_allocator_type>;
		using list_map__list_map_base_type =
			::ZipDirFs::Containers::list_map<std::string, std::int32_t>;
		list_map__list_map_mocked_base_type* list_map__list_map_mocked_base_ref;
		list_map__list_mock_helper_type* list_map__list_mock_helper_ref;
		list_map__map_mock_helper_type* list_map__map_mock_helper_ref;
		list_map__list_map_base_type* list_map__list_map_base_ref;
		using EntryList__EntryListMockedBase_type =
			::ZipDirFs::Containers::EntryList<std::less<std::string>, EntryList___Alloc,
				std::allocator<std::string>, list_map_mock>;
		using EntryList__EntryListForProxyMockedBase_type =
			::ZipDirFs::Containers::EntryList<std::less<std::string>, EntryList___Alloc,
				std::allocator<std::string>, list_map_for_proxy>;
		EntryList__EntryListMockedBase_type& EntryList__EntryListMockedBase_ref;
		EntryList__EntryListForProxyMockedBase_type& EntryList__EntryListForProxyMockedBase_ref;
	};
} // namespace
