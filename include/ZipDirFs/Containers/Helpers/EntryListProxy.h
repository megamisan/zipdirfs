/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_CONTAINERS_HELPERS_ENTRYLISTPROXY_H
#define ZIPDIRFS_CONTAINERS_HELPERS_ENTRYLISTPROXY_H

#include "ZipDirFs/Containers/EntryIterator.h"
#include <memory>
#include <string>

namespace fusekit
{
	struct entry;
}

namespace ZipDirFs::Containers
{
	namespace Helpers
	{
		/**
		 * @brief A proxy definition to an @link ZipDirFs::Containers::EntryList EntryList @endlink without irrelevant templating information
		 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
		 */
		class EntryListProxy
		{
		public:
			typedef EntryIterator iterator;
			typedef std::string key_type;
			typedef fusekit::entry* mapped_type;
			virtual ~EntryListProxy() {}
			virtual iterator begin() = 0;
			virtual iterator end() = 0;
			virtual iterator insert(iterator, const key_type&, const mapped_type&) = 0;
			virtual iterator erase(iterator) = 0;
			virtual mapped_type find(const key_type& name) const = 0;
		};
	} // namespace Helpers
} // namespace ZipDirFs::Containers

#endif // ZIPDIRFS_CONTAINERS_HELPERS_ENTRYLISTPROXY_H
