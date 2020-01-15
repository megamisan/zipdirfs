/*
 * Copyright © 2019-2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_LIST_MAP_H_INCLUDED
#define ZIPDIRFS_LIST_MAP_H_INCLUDED

#include <list>
#include <map>

namespace ZipDirFs::Containers
{
	/**
	 * A container assembling a list and a map.
	 *
	 * The list maintains the element's order. The map associates the elements
	 * with their values.
	 */
	template <class Key, class T, class Compare = std::less<Key>,
		class MapAllocator = std::allocator<std::pair<const Key, T>>,
		class ListAllocator = std::allocator<Key>, class List = std::list<Key, ListAllocator>,
		class Map = std::map<Key, T, Compare, MapAllocator>>
	class list_map
	{
	public:
		typedef List list_type;
		typedef Map map_type;
		typedef Key key_type;
		typedef T mapped_type;
		typedef typename map_type::value_type value_type;
		typedef Compare compare_type;
		typedef MapAllocator map_allocator;
		typedef ListAllocator list_allocator;

		// construct/copy/destroy:
		explicit list_map(const Compare& comp = Compare(),
			const MapAllocator& map_alloc = MapAllocator(),
			const ListAllocator& list_alloc = ListAllocator()) :
			_list(list_alloc),
			_map(comp, map_alloc)
		{
		}
		~list_map(){};
		typename list_type::allocator_type get_list_allocator() const _GLIBCXX_NOEXCEPT
		{
			return _list.get_allocator();
		}
		typename map_type::allocator_type get_map_allocator() const _GLIBCXX_NOEXCEPT
		{
			return _map.get_allocator();
		}

		// iterators:
		typename list_type::iterator begin() _GLIBCXX_NOEXCEPT { return _list.begin(); }
		typename list_type::const_iterator begin() const _GLIBCXX_NOEXCEPT { return _list.begin(); }
		typename list_type::iterator end() _GLIBCXX_NOEXCEPT { return _list.end(); }
		typename list_type::const_iterator end() const _GLIBCXX_NOEXCEPT { return _list.end(); }

		typename list_type::reverse_iterator rbegin() _GLIBCXX_NOEXCEPT { return _list.rbegin(); }
		typename list_type::const_reverse_iterator rbegin() const _GLIBCXX_NOEXCEPT
		{
			return _list.rbegin();
		}
		typename list_type::reverse_iterator rend() _GLIBCXX_NOEXCEPT { return _list.rend(); }
		typename list_type::const_reverse_iterator rend() const _GLIBCXX_NOEXCEPT
		{
			return _list.rend();
		}

#if __cplusplus >= 201103L
		typename list_type::const_iterator cbegin() const noexcept { return _list.cbegin(); }
		typename list_type::const_iterator cend() const noexcept { return _list.cend(); }
		typename list_type::const_reverse_iterator crbegin() const noexcept
		{
			return _list.crbegin();
		}
		typename list_type::const_reverse_iterator crend() const noexcept { return _list.crend(); }
#endif

		// capacity:
		typename list_type::size_type size() const _GLIBCXX_NOEXCEPT { return _list.size(); }
		bool empty() const _GLIBCXX_NOEXCEPT { return _list.empty(); }

		// element access:
		typename list_type::reference front() { return _list.front(); }
		typename list_type::const_reference front() const { return _list.front(); }
		typename list_type::reference back() { return _list.back(); }
		typename list_type::const_reference back() const { return _list.back(); }

		// modifiers:
#if __cplusplus >= 201103L
		typename list_type::iterator insert(
			typename list_type::const_iterator position, const key_type& k, const T& x)
#else
		typename list_type::iterator insert(
			typename list_type::iterator position, const key_type& k, const T& x)
#endif
		{
			_map.insert(std::move(value_type(k, x)));
			return _list.insert(position, k);
		}
#if __cplusplus >= 201103L
		typename list_type::iterator erase(typename list_type::const_iterator position)
#else
		typename list_type::iterator erase(typename list_type::iterator position)
#endif
		{
			_map.erase(*position);
			return _list.erase(position);
		}

		// map operations:
		typename map_type::iterator find(const key_type& x) { return _map.find(x); }
		typename map_type::const_iterator find(const key_type& x) const { return _map.find(x); }
		bool valid(typename map_type::iterator it) { return it != _map.end(); }
		bool valid(typename map_type::const_iterator it) const { return it != _map.end(); }

	protected:
		list_type _list;
		map_type _map;
	};
} // namespace ZipDirFs::Containers

#endif // ZIPDIRFS_LIST_MAP_H_INCLUDED
