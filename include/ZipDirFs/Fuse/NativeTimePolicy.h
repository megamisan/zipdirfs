/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_FUSE_NATIVETIMEPOLICY_H
#define ZIPDIRFS_FUSE_NATIVETIMEPOLICY_H

#include <ctime>

namespace ZipDirFs::Fuse
{
	using std::time_t;

	/**
	 * @brief A fusekit time policy for real filesystem entries
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 * @tparam Derived Recursive templating parameter.
	 */
	template <class Derived>
	struct NativeTimePolicy
	{
		timespec change_time() { return (timespec){this->getChangeTime()}; }
		timespec access_time() { return (timespec){this->getModificationTime()}; }
		timespec modification_time() { return (timespec){this->getModificationTime()}; }
		void update(int) {}
		virtual std::time_t getChangeTime() const = 0;
		virtual std::time_t getModificationTime() const = 0;
	};
} // namespace ZipDirFs::Fuse

#endif // ZIPDIRFS_FUSE_NATIVETIMEPOLICY_H
