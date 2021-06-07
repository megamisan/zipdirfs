/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_FIXTURES_MAP_MOCK_H_INCLUDED
#define TEST_ZIPDIRFS_CONTAINERS_FIXTURES_MAP_MOCK_H_INCLUDED

#include <functional>
#include <gmock/gmock.h>

namespace Test::ZipDirFs::Containers
{
	namespace Fixtures
	{
		template <typename _Compare, typename _Alloc>
		struct map_mock_helper
		{
			MOCK_METHOD2_T(create, void(const _Compare* __comp, const _Alloc* __a));
			static map_mock_helper<_Compare, _Alloc>* instance;
			map_mock_helper() { instance = this; }
			~map_mock_helper() { instance = NULL; }
		};
		template <typename _Compare, typename _Alloc>
		map_mock_helper<_Compare, _Alloc>* map_mock_helper<_Compare, _Alloc>::instance;

		template <typename _Key, typename _Tp, typename _Compare = std::less<_Key>,
			typename _Alloc = std::allocator<std::pair<const _Key, _Tp>>>
		class map_mock
		{
		public:
			typedef _Key key_type;
			typedef _Tp mapped_type;
			typedef std::pair<const _Key, _Tp> value_type;
			typedef _Compare key_compare;
			typedef _Alloc allocator_type;

		protected:
			typedef typename __gnu_cxx::__alloc_traits<_Alloc>::template rebind<value_type>::other
				_Pair_alloc_type;
			typedef std::_Rb_tree<key_type, value_type, std::_Select1st<value_type>, key_compare,
				_Pair_alloc_type>
				_Rep_type;

		public:
			typedef typename _Rep_type::iterator iterator;
			typedef typename _Rep_type::const_iterator const_iterator;
			typedef typename _Rep_type::size_type size_type;
			explicit map_mock(const _Compare& __comp, const allocator_type& __a = allocator_type())
			{
				if (map_mock_helper<_Compare, _Alloc>::instance != NULL)
					map_mock_helper<_Compare, _Alloc>::instance->create(&__comp, &__a);
			}
			MOCK_CONST_METHOD0_T(get_allocator, allocator_type());
			MOCK_METHOD0_T(end, iterator());
			MOCK_CONST_METHOD0_T(end, const_iterator());
			MOCK_CONST_METHOD1_T(insert, std::pair<iterator, bool>(const value_type& __x));
			MOCK_CONST_METHOD1_T(erase, size_type(const key_type& __x));
			MOCK_METHOD1_T(find, iterator(const key_type& __x));
			MOCK_CONST_METHOD1_T(find, const_iterator(const key_type& __x));
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers

#endif // TEST_ZIPDIRFS_CONTAINERS_FIXTURES_MAP_MOCK_H_INCLUDED
