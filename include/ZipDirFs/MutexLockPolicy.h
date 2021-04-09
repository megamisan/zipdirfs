/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_MUTEXLOCKPOLICY_H
#define ZIPDIRFS_MUTEXLOCKPOLICY_H

namespace ZipDirFs
{
	/**
	 * \brief Represents the mutex, internally.
	 */
	struct mutex;

	/**
	 * \brief Provides locking facilities using mutex.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class MutexLockPolicy
	{
	public:
		MutexLockPolicy();
		~MutexLockPolicy();
		class UnLock;
		/**
		 * \brief Provides scoped locking facility.
		 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
		 */
		class Lock
		{
		public:
			Lock(MutexLockPolicy& policy);
			~Lock();
		private:
			MutexLockPolicy& policy;
			friend class UnLock;
		};
		/**
		 * \brief Provides scoped unlocking facility.
		 * Only works on an already defined lock.
		 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
		 */
		class UnLock
		{
		public:
			UnLock(Lock& lock);
			~UnLock();
		private:
			Lock &lock;
		};
		typedef Lock lock;
	protected:
	private:
		struct mutex* mutex;
		friend class Lock;
	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_MUTEXLOCKPOLICY_H
