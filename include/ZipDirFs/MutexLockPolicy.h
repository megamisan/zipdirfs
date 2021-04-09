/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 *
 * This file is part of zipdirfs.
 *
 * zipdirfs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * zipdirfs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with zipdirfs.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MUTEXLOCKPOLICY_H
#define MUTEXLOCKPOLICY_H

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
}

#endif // MUTEXLOCKPOLICY_H
