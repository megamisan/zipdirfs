/*
 * Copyright © 2012 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
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
 *
 * $Id$
 */
#ifndef MUTEXLOCKPOLICY_H
#define MUTEXLOCKPOLICY_H

namespace zipdirfs
{
	struct mutex;
	class MutexLockPolicy
	{
	public:
		MutexLockPolicy();
		~MutexLockPolicy();
		class Lock
		{
		public:
			Lock (MutexLockPolicy& policy);
			~Lock();
		private:
			MutexLockPolicy& policy;
		};
		typedef Lock lock;
	protected:
	private:
		struct mutex* mutex;
		friend class Lock;
	};
}

#endif // MUTEXLOCKPOLICY_H
