/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Utilities/list_cleanup.h"

namespace ZipDirFs::Utilities
{
	void CleanupEntryList(::ZipDirFs::Containers::Helpers::EntryListProxy& proxy,
		::ZipDirFs::Containers::Helpers::Factory& factory)
	{
		auto it = proxy.begin(), endIt = proxy.end();
		while (!(it == endIt))
		{
			factory.destroy(proxy.find(*it));
			it = proxy.erase(it);
		}
	}
} // namespace ZipDirFs::Utilities
