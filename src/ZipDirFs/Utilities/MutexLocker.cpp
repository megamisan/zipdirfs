/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Utilities/MutexLocker.h"
#include "StateReporter.h"

namespace ZipDirFs::Utilities
{
	MutexLocker::MutexLocker() { id = buildId("ML", (std::uint64_t)(void*)&mutex, 6); }
	std::unique_lock<std::mutex> MutexLocker::lock() { return std::unique_lock<std::mutex>(mutex); }
	MutexLocker::operator const std::string&() const { return id; }
} // namespace ZipDirFs::Utilities
