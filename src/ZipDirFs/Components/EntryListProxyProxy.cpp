/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Components/EntryListProxyProxy.h"

namespace ZipDirFs::Components
{
	EntryListProxyProxy::EntryListProxyProxy(parent_ptr&& entryListProxy) :
		entryListProxy(std::move(entryListProxy))
	{
	}

	void EntryListProxyProxy::swap(parent_ptr& other) { entryListProxy.swap(other); }

	EntryListProxyProxy::iterator EntryListProxyProxy::begin() { return entryListProxy->begin(); }
	EntryListProxyProxy::iterator EntryListProxyProxy::end() { return entryListProxy->end(); }
	EntryListProxyProxy::iterator EntryListProxyProxy::insert(
		iterator it, const key_type& k, const mapped_type& m)
	{
		return entryListProxy->insert(it, k, m);
	}
	EntryListProxyProxy::iterator EntryListProxyProxy::erase(iterator it)
	{
		return entryListProxy->erase(it);
	}
	EntryListProxyProxy::mapped_type EntryListProxyProxy::find(const key_type& name) const
	{
		return entryListProxy->find(name);
	}

} // namespace ZipDirFs::Components
