/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Utilities/MutexLocker.h"

namespace ZipDirFs::Utilities
{
	std::unique_lock<std::mutex> MutexLocker::lock() { return std::unique_lock<std::mutex>(mutex); }
} // namespace ZipDirFs::Utilities
