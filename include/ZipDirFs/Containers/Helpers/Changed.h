/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_CONTAINERS_HELPERS_CHANGED_H
#define ZIPDIRFS_CONTAINERS_HELPERS_CHANGED_H

#include <ctime>

namespace ZipDirFs::Containers
{
	namespace Helpers
	{
		/**
		 * @brief An invokable definition to know if the observed object has changed
		 * @remarks State is reset on invocation. The changed state can only be captured once per
		 * change.
		 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
		 */
		class Changed
		{
		public:
			virtual ~Changed() {}
			virtual bool operator()() = 0;
			virtual operator std::time_t() const = 0;
		};
	} // namespace Helpers
} // namespace ZipDirFs::Containers

#endif // ZIPDIRFS_CONTAINERS_HELPERS_CHANGED_H
