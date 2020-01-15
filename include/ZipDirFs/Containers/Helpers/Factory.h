/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_CONTAINERS_HELPERS_FACTORY_H
#define ZIPDIRFS_CONTAINERS_HELPERS_FACTORY_H

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
		 * A factory definition to manage list entries
		 */
		class Factory
		{
		public:
			typedef std::string key_type;
			typedef fusekit::entry* mapped_type;
			virtual ~Factory() {}
			virtual const mapped_type create(const key_type& name) = 0;
			virtual void destroy(const mapped_type& item) = 0;
		};
	} // namespace Helpers
} // namespace ZipDirFs::Containers

#endif // ZIPDIRFS_CONTAINERS_HELPERS_FACTORY_H
