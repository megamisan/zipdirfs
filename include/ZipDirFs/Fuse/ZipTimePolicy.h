/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_FUSE_ZIPTIMEPOLICY_H
#define ZIPDIRFS_FUSE_ZIPTIMEPOLICY_H

#include <ctime>

namespace ZipDirFs::Fuse
{
	using std::time_t;

	template <class Derived>
	struct ZipTimePolicy
	{
		timespec change_time() { return (timespec){this->getParentModificationTime()}; }
		timespec access_time() { return (timespec){this->getModificationTime()}; }
		timespec modification_time() { return (timespec){this->getModificationTime()}; }
		void update(int) {}
		virtual std::time_t getParentModificationTime() const = 0;
		virtual std::time_t getModificationTime() const = 0;
	};
} // namespace ZipDirFs::Fuse

#endif // ZIPDIRFS_FUSE_ZIPTIMEPOLICY_H
