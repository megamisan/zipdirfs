/*
 * Copyright © 2012-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_UTILITIES_MUTEXLOCKER_H
#define ZIPDIRFS_UTILITIES_MUTEXLOCKER_H

#include <mutex>

namespace ZipDirFs::Utilities
{
	/**
	 * @brief Provides unique locking feature
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class MutexLocker
	{
	public:
		MutexLocker();
		std::unique_lock<std::mutex> lock();
		operator const std::string&() const;

	protected:
	private:
		std::mutex mutex;
		std::string id;
	};
} // namespace ZipDirFs::Utilities

#endif // ZIPDIRFS_UTILITIES_MUTEXLOCKER_H
