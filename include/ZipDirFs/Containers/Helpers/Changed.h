/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_CONTAINERS_HELPERS_CHANGED_H
#define ZIPDIRFS_CONTAINERS_HELPERS_CHANGED_H

namespace ZipDirFs::Containers
{
	namespace Helpers
	{
		/**
		 * An invokable definition to know if the observed object has changed.
		 *
		 * @remarks State is reset on invocation. The changed state can only be captured once per change.
		 */
		class Changed
		{
		public:
			virtual ~Changed() {}
			virtual bool operator()() = 0;
		};
	} // namespace Helpers
} // namespace ZipDirFs::Containers

#endif // ZIPDIRFS_CONTAINERS_HELPERS_CHANGED_H
