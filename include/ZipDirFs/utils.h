/*
 * Copyright © 2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_UTILS_H
#define ZIPDIRFS_UTILS_H

#include <ctime>

namespace ZipDirFs
{
	inline bool equals(const struct timespec& a, const struct timespec& b)
	{
		return a.tv_sec == b.tv_sec && a.tv_nsec == b.tv_nsec;
	}
} // namespace ZipDirFs

#endif // ZIPDIRFS_UTILS_H
