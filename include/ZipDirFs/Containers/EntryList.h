/*
 * Copyright © 2019-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_CONTAINERS_ENTRYLIST_H
#define ZIPDIRFS_CONTAINERS_ENTRYLIST_H

#include "ZipDirFs/Containers/EntryIterator.h"
#include "ZipDirFs/Containers/Helpers/EntryListProxy.h"
#include "ZipDirFs/Containers/list_map.h"
#include <type_traits>

namespace fusekit
{
	class entry;
}

namespace ZipDirFs::Containers
{
	/**
	 * @brief A templatable list of named fusekit entries
	 * @remarks A non templated proxy to the list can be obtained using @link ZipDirFs::Containers::EntryList::createWithProxy createWithProxy() @endlink.
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 * @tparam Compare A binary predicate that takes two element keys as arguments and returns a `bool`. The predicate shall return `true` if its first argument should go before its second.
	 * @tparam MapAllocator Type of the allocator object used to define the storage allocation model for the underlying map.
	 * @tparam ListAllocator Type of the allocator object used to define the storage allocation model for the underlying list.
	 * @tparam ListMap Type of the underlying combined list and map object.
	 */
	template <typename Compare = std::less<std::string>,
		typename MapAllocator = std::allocator<std::pair<const std::string, fusekit::entry*>>,
		typename ListAllocator = std::allocator<std::string>,
		typename ListMap =
			list_map<std::string, fusekit::entry*, Compare, MapAllocator, ListAllocator>>
	class EntryList
	{
		using Self = EntryList<Compare, MapAllocator, ListAllocator, ListMap>;

	protected:
		class Wrapper;
		class Proxy;

	public:
		typedef typename ListMap::key_type key_type;
		typedef typename ListMap::value_type value_type;
		typedef typename ListMap::mapped_type mapped_type;
		typedef typename ListMap::list_type::iterator iterator;
		typedef std::shared_ptr<Helpers::EntryListProxy> proxy_ptr;
		iterator begin() { return container.begin(); }
		iterator end() { return container.end(); };
		iterator insert(iterator position, const key_type& name, const mapped_type& entry)
		{
			return container.insert(position, name, entry);
		}
		iterator erase(iterator position) { return container.erase(position); }
		const mapped_type find(const key_type& name) const
		{
			auto it = container.find(name);
			return container.valid(it) ? it->second : nullptr;
		}
		static proxy_ptr createWithProxy() { return proxy_ptr(new Proxy()); }

	protected:
		ListMap container;
		using WrapperBase = EntryIterator::Wrapper;

		class Wrapper : public WrapperBase
		{
		public:
			typedef Self::iterator iterator;
			Wrapper(iterator&& it) : _real(std::move(it)) {}
			Wrapper(const iterator& it) : _real(it) {}
			~Wrapper(){};

		protected:
			WrapperBase* clone() const { return new Wrapper(_real); };
			reference dereference() const { return *_real; };
			void increment() { ++_real; };
			bool equals(const WrapperBase& w) const
			{
				return dynamic_cast<const Wrapper*>(&w) != nullptr
					&& _real == static_cast<const Wrapper*>(&w)->_real;
			};
			iterator _real;
			friend class Proxy;
		};
		class Proxy : public Helpers::EntryListProxy
		{
		public:
			using iterator_proxy = Helpers::EntryListProxy::iterator;
			Proxy() {}
			~Proxy() {}
			iterator_proxy begin() { return iterator_proxy(new Wrapper(_list.begin())); };
			iterator_proxy end() { return iterator_proxy(new Wrapper(_list.end())); };
			iterator_proxy insert(iterator_proxy it, const key_type& key, const mapped_type& value)
			{
				return iterator_proxy(new Wrapper(_list.insert(
					static_cast<const Wrapper* const>(it.getWrapper())->_real, key, value)));
			};
			iterator_proxy erase(iterator_proxy it)
			{
				return iterator_proxy(new Wrapper(
					_list.erase(static_cast<const Wrapper* const>(it.getWrapper())->_real)));
			};
			fusekit::entry* find(const key_type& name) const { return _list.find(name); };

		protected:
			Self _list;
		};

	private:
	};
} // namespace ZipDirFs::Containers

#endif // ZIPDIRFS_CONTAINERS_ENTRYLIST_H
