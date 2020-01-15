/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_FIXTURES_LIST_MOCK_H_INCLUDED
#define TEST_ZIPDIRFS_CONTAINERS_FIXTURES_LIST_MOCK_H_INCLUDED

#include <functional>
#include <gmock/gmock.h>

namespace Test::ZipDirFs::Containers
{
	namespace Fixtures
	{
		template <typename _Alloc>
		struct list_mock_helper
		{
			MOCK_METHOD1_T(create, void(const _Alloc* __a));
			static list_mock_helper<_Alloc>* instance;
			list_mock_helper() { instance = this; }
			~list_mock_helper() { instance = nullptr; }
		};
		template <typename _Alloc>
		list_mock_helper<_Alloc>* list_mock_helper<_Alloc>::instance;

		template <typename _Tp, typename _Alloc = std::allocator<_Tp>>
		class list_mock
		{
		protected:
			typedef typename __gnu_cxx::__alloc_traits<_Alloc>::template rebind<_Tp>::other
				_Tp_alloc_type;
			typedef __gnu_cxx::__alloc_traits<_Tp_alloc_type> _Tp_alloc_traits;

		public:
			typedef _Tp value_type;
			typedef typename _Tp_alloc_traits::reference reference;
			typedef typename _Tp_alloc_traits::const_reference const_reference;
			typedef std::_List_iterator<_Tp> iterator;
			typedef std::_List_const_iterator<_Tp> const_iterator;
			typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
			typedef std::reverse_iterator<iterator> reverse_iterator;
			typedef size_t size_type;
			typedef _Alloc allocator_type;
			explicit list_mock(const allocator_type& __a) _GLIBCXX_NOEXCEPT
			{
				if (list_mock_helper<_Alloc>::instance != nullptr)
					list_mock_helper<_Alloc>::instance->create(&__a);
			}
			MOCK_CONST_METHOD0_T(get_allocator, allocator_type());
			MOCK_METHOD0_T(begin, iterator());
			MOCK_CONST_METHOD0_T(begin, const_iterator());
			MOCK_METHOD0_T(end, iterator());
			MOCK_CONST_METHOD0_T(end, const_iterator());
			MOCK_METHOD0_T(rbegin, reverse_iterator());
			MOCK_CONST_METHOD0_T(rbegin, const_reverse_iterator());
			MOCK_METHOD0_T(rend, reverse_iterator());
			MOCK_CONST_METHOD0_T(rend, const_reverse_iterator());
#if __cplusplus >= 201103L
			MOCK_CONST_METHOD0_T(cbegin, const_iterator());
			MOCK_CONST_METHOD0_T(cend, const_iterator());
			MOCK_CONST_METHOD0_T(crbegin, const_reverse_iterator());
			MOCK_CONST_METHOD0_T(crend, const_reverse_iterator());
#endif
			MOCK_CONST_METHOD0_T(empty, bool());
			MOCK_CONST_METHOD0_T(size, size_type());
			MOCK_METHOD0_T(front, reference());
			MOCK_CONST_METHOD0_T(front, const_reference());
			MOCK_METHOD0_T(back, reference());
			MOCK_CONST_METHOD0_T(back, const_reference());
#if __cplusplus >= 201103L
			MOCK_METHOD2_T(insert, iterator(const_iterator __position, const value_type& __x));
#else
			MOCK_METHOD2_T(insert, iterator(iterator __position, const value_type& __x));
#endif
#if __cplusplus >= 201103L
			MOCK_METHOD1_T(erase, iterator(const_iterator __position));
#else
			MOCK_METHOD1_T(erase, iterator(iterator __position));
#endif
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers

#endif // TEST_ZIPDIRFS_CONTAINERS_FIXTURES_LIST_MOCK_H_INCLUDED
