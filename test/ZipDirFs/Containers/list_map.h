/*
 * Copyright © 2019-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_LIST_MAP_H_INCLUDED
#define TEST_ZIPDIRFS_CONTAINERS_LIST_MAP_H_INCLUDED

#include "Fixtures/list_mock.h"
#include "Fixtures/map_mock.h"

namespace Test::ZipDirFs::Containers
{
	using Fixtures::list_mock;
	using Fixtures::list_mock_helper;
	using Fixtures::map_mock;
	using Fixtures::map_mock_helper;
	using list_type = list_mock<std::string>;
	using map_type = map_mock<std::string, std::int32_t>;
	using compare_type = map_type::key_compare;
	using list_allocator_type = list_type::allocator_type;
	using map_allocator_type = map_type::allocator_type;
	using list_map_mocked = ::ZipDirFs::Containers::list_map<std::string, std::int32_t,
		compare_type, map_allocator_type, list_allocator_type, list_type, map_type>;
	using list_mock_helper_type = list_mock_helper<list_allocator_type>;
	using map_mock_helper_type = map_mock_helper<compare_type, map_allocator_type>;
	using list_map = ::ZipDirFs::Containers::list_map<std::string, std::int32_t>;

	struct list_map_mocked_access : public list_map_mocked
	{
		static list_map_mocked::list_type& list(list_map_mocked&);
		static list_map_mocked::map_type& map(list_map_mocked&);
		static const list_map_mocked::list_type& list(const list_map_mocked&);
		static const list_map_mocked::map_type& map(const list_map_mocked&);
	};

	class list_map_mock_test : public ::testing::Test
	{
	protected:
		typename list_map_mocked_access::compare_type compare;
		typename list_map_mocked_access::list_allocator list_allocator;
		typename list_map_mocked_access::map_allocator map_allocator;
	};

	class list_map_test : public ::testing::Test
	{
	public:
		list_map_test();

	protected:
		void SetUp();
		static const std::string generateKey();
		const std::int32_t first_value;
		const std::int32_t second_value;
		const std::string first_key;
		const std::string second_key;
		list_map::list_type::iterator first_iterator;
		list_map::list_type::iterator second_iterator;
		list_map lm;
	};

} // namespace Test::ZipDirFs::Containers

#endif // TEST_ZIPDIRFS_CONTAINERS_LIST_MAP_H_INCLUDED
