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
#include "zipdirfs/MutexLockPolicy.h"
#include <pthread.h>

namespace zipdirfs
{
	struct mutex
	{
		pthread_mutex_t _mutex;
	};

	MutexLockPolicy::MutexLockPolicy()
	{
		this->mutex = new struct mutex();
		pthread_mutex_init (&this->mutex->_mutex, NULL);
	}

	MutexLockPolicy::~MutexLockPolicy()
	{
		pthread_mutex_destroy (&this->mutex->_mutex);
		delete this->mutex;
	}

	MutexLockPolicy::Lock::~Lock()
	{
		pthread_mutex_unlock (&this->policy.mutex->_mutex);
	}

	MutexLockPolicy::Lock::Lock (MutexLockPolicy& policy) : policy (policy)
	{
		pthread_mutex_lock (&this->policy.mutex->_mutex);
	}

	MutexLockPolicy::UnLock::~UnLock()
	{
		pthread_mutex_lock (&this->lock.policy.mutex->_mutex);
	}

	MutexLockPolicy::UnLock::UnLock (MutexLockPolicy::Lock& lock) : lock (lock)
	{
		pthread_mutex_unlock (&this->lock.policy.mutex->_mutex);
	}
}
