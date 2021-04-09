/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/SymlinkTime.h"

namespace ZipDirFs
{
	inline struct timespec clock_now()
	{
		struct timespec tv;
		clock_gettime(CLOCK_REALTIME, &tv);
		return tv;
	}

	const struct timespec symlinksTime = clock_now();
} // namespace ZipDirFs
