/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_FIXTURES_LIST_FOR_PROXY_H_INCLUDED
#define TEST_ZIPDIRFS_CONTAINERS_FIXTURES_LIST_FOR_PROXY_H_INCLUDED

#include <functional>
#include <gmock/gmock.h>

namespace fusekit
{
	struct entry;
}

namespace Test::ZipDirFs::Containers
{
	namespace Fixtures
	{
		template <typename _Tp, typename _Alloc = std::allocator<_Tp>>
		struct list_for_proxy;

		template <typename _Alloc>
		struct list_for_proxy_create_helper
		{
			MOCK_METHOD1_T(create, void(const _Alloc* __a));
			static list_for_proxy_create_helper<_Alloc>* instance;
			list_for_proxy_create_helper() { instance = this; }
			~list_for_proxy_create_helper() { instance = nullptr; }
		};
		template <typename _Alloc>
		list_for_proxy_create_helper<_Alloc>* list_for_proxy_create_helper<_Alloc>::instance;

		template <typename _Tp>
		struct list_for_proxy_destruct_helper
		{
			MOCK_METHOD1_T(destruct, void(list_for_proxy<_Tp>*));
			static list_for_proxy_destruct_helper<_Tp>* instance;
			list_for_proxy_destruct_helper() { instance = this; }
			~list_for_proxy_destruct_helper() { instance = nullptr; }
		};
		template <typename _Tp>
		list_for_proxy_destruct_helper<_Tp>* list_for_proxy_destruct_helper<_Tp>::instance;

		struct list_for_proxy_iterator_equals_helper
		{
			MOCK_METHOD0_T(equals, void());
			static list_for_proxy_iterator_equals_helper* instance;
			list_for_proxy_iterator_equals_helper();
			~list_for_proxy_iterator_equals_helper();
		};

		template <typename _Tp, typename _Alloc>
		struct list_for_proxy
		{
		protected:
			typedef typename __gnu_cxx::__alloc_traits<_Alloc>::template rebind<_Tp>::other
				_Tp_alloc_type;
			typedef __gnu_cxx::__alloc_traits<_Tp_alloc_type> _Tp_alloc_traits;

		public:
			typedef _Tp value_type;
			typedef typename _Tp_alloc_traits::reference reference;
			typedef typename _Tp_alloc_traits::const_reference const_reference;
			typedef size_t size_type;
			typedef _Alloc allocator_type;
			struct iterator
			{
				iterator() : source(nullptr), position(-1) {}
				iterator(list_for_proxy* s, size_type p) : source(s), position(p) {}
				iterator(iterator&& it)
				{
					source = it.source;
					position = it.position;
				}
				iterator(const iterator& it)
				{
					source = it.source;
					position = it.position;
				}
				iterator& operator=(iterator&& it)
				{
					source = it.source;
					position = it.position;
				}
				iterator& operator=(const iterator& it)
				{
					source = it.source;
					position = it.position;
				}
				iterator& operator++()
				{
					position++;
					if (position == source->size())
						position = UINT64_MAX;
					return *this;
				}
				bool operator==(const iterator& it) const
				{
					if (list_for_proxy_iterator_equals_helper::instance != nullptr)
					{
						list_for_proxy_iterator_equals_helper::instance->equals();
					}
					return source == it.source && position == it.position;
				}
				const _Tp& operator*() const { return source->getValue(position); }
				const list_for_proxy* getSource() const { return source; }
				const size_type getPosition() const { return position; }

			protected:
				list_for_proxy* source;
				size_type position;
			};
			typedef iterator const_iterator;
			typedef iterator reverse_iterator;
			typedef iterator const_reverse_iterator;
			explicit list_for_proxy(const allocator_type& __a) _GLIBCXX_NOEXCEPT
			{
				if (list_for_proxy_create_helper<_Alloc>::instance != nullptr)
					list_for_proxy_create_helper<_Alloc>::instance->create(&__a);
			}
			~list_for_proxy()
			{
				if (list_for_proxy_destruct_helper<std::string>::instance != nullptr)
					list_for_proxy_destruct_helper<std::string>::instance->destruct(this);
			}
			MOCK_METHOD0_T(begin, iterator());
			MOCK_METHOD0_T(end, iterator());
			MOCK_METHOD2_T(insert, iterator(iterator __position, const value_type& __x));
			MOCK_METHOD1_T(erase, iterator(iterator __position));
			MOCK_CONST_METHOD0_T(size, size_type());
			MOCK_CONST_METHOD1_T(getValue, const _Tp&(const size_type));
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers

#endif // TEST_ZIPDIRFS_CONTAINERS_FIXTURES_LIST_FOR_PROXY_H_INCLUDED
