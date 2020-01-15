/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_FIXTURES_LIST_MAP_MOCK_H_INCLUDED
#define TEST_ZIPDIRFS_CONTAINERS_FIXTURES_LIST_MAP_MOCK_H_INCLUDED

#include "ZipDirFs/Containers/list_map.h"
#include "list_mock.h"
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
		struct list_map_mock
		{
			typedef typename std::list<std::string> list_type;
			typedef typename std::map<std::string, fusekit::entry*> map_type;
			typedef map_type::key_type key_type;
			typedef map_type::value_type value_type;
			typedef map_type::mapped_type mapped_type;
			MOCK_METHOD0(begin, list_type::iterator());
			MOCK_METHOD0(end, list_type::iterator());
#if __cplusplus >= 201103L
			MOCK_METHOD3(insert,
				list_type::iterator(list_type::const_iterator pos, const map_type::key_type& name,
					const map_type::mapped_type& entry));
			MOCK_METHOD1(erase, list_type::iterator(list_type::const_iterator pos));
#else
			MOCK_METHOD3(insert,
				list_type::iterator(list_type::iterator pos, const map_type::key_type& name,
					const map_type::mapped_type& entry));
			MOCK_METHOD1(erase, list_type::iterator(list_type::iterator pos));
#endif
			MOCK_CONST_METHOD1(find, map_type::iterator(const map_type::key_type& name));
			MOCK_CONST_METHOD1(valid, bool(map_type::iterator mit));
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers

#endif // TEST_ZIPDIRFS_CONTAINERS_FIXTURES_LIST_MAP_MOCK_H_INCLUDED
