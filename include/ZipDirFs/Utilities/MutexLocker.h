/*
 * Copyright © 2012-2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_UTILITIES_MUTEXLOCKER_H
#define ZIPDIRFS_UTILITIES_MUTEXLOCKER_H

#include <mutex>

namespace ZipDirFs::Utilities
{
	/**
	 * \brief Provides unique locking feature
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class MutexLocker
	{
	public:
		std::unique_lock<std::mutex> lock();

	protected:
	private:
		std::mutex mutex;
	};
} // namespace ZipDirFs::Utilities

#endif // ZIPDIRFS_UTILITIES_MUTEXLOCKER_H
