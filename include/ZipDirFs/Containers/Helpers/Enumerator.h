/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_CONTAINERS_HELPERS_ENUMERATOR_H
#define ZIPDIRFS_CONTAINERS_HELPERS_ENUMERATOR_H

namespace ZipDirFs::Containers
{
	namespace Helpers
	{
		/**
		 * @brief A templated enumerator definition
		 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
		 * @tparam T Value type
		 */
		template <typename T>
		class Enumerator
		{
		public:
			virtual ~Enumerator() {}
			virtual void reset() = 0;
			virtual void next() = 0;
			virtual bool valid() = 0;
			virtual const T& current() = 0;
		};

	} // namespace Helpers
} // namespace ZipDirFs::Containers

#endif // ZIPDIRFS_CONTAINERS_HELPERS_ENUMERATOR_H
