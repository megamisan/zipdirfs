/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Components/EntryListProxyIncremental.h"

namespace ZipDirFs::Components
{
	EntryListProxyIncremental::EntryListProxyIncremental(
		std::function<mapped_type(const key_type&)>&& resolve,
		std::shared_ptr<ZipDirFs::Containers::Helpers::EntryListProxy>&& entryListProxy) :
		entryListProxy(std::move(entryListProxy)),
		resolve(resolve)
	{
	}

	const std::shared_ptr<ZipDirFs::Containers::Helpers::EntryListProxy>&
		EntryListProxyIncremental::get() const
	{
		return entryListProxy;
	}

	EntryListProxyIncremental::iterator EntryListProxyIncremental::begin()
	{
		return entryListProxy->begin();
	}
	EntryListProxyIncremental::iterator EntryListProxyIncremental::end()
	{
		return entryListProxy->end();
	}
	EntryListProxyIncremental::iterator EntryListProxyIncremental::insert(
		iterator it, const key_type& k, const mapped_type& m)
	{
		return entryListProxy->insert(it, k, m);
	}
	EntryListProxyIncremental::iterator EntryListProxyIncremental::erase(iterator it)
	{
		return entryListProxy->erase(it);
	}
	EntryListProxyIncremental::mapped_type EntryListProxyIncremental::find(
		const key_type& name) const
	{
		auto entry = entryListProxy->find(name);
		if (entry == nullptr)
		{
			entry = resolve(name);
			if (entry != nullptr)
			{
				entryListProxy->insert(entryListProxy->end(), name, entry);
			}
		}
		return entry;
	}

} // namespace ZipDirFs::Components
