/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/MutexLockPolicy.h"
#include <pthread.h>

namespace ZipDirFs
{
	struct mutex
	{
		pthread_mutex_t _mutex;
	};

	MutexLockPolicy::MutexLockPolicy()
	{
		this->mutex = new struct mutex();
		pthread_mutex_init(&this->mutex->_mutex, NULL);
	}

	MutexLockPolicy::~MutexLockPolicy()
	{
		pthread_mutex_destroy(&this->mutex->_mutex);
		delete this->mutex;
	}

	MutexLockPolicy::Lock::~Lock()
	{
		pthread_mutex_unlock(&this->policy.mutex->_mutex);
	}

	MutexLockPolicy::Lock::Lock(MutexLockPolicy& policy) : policy(policy)
	{
		pthread_mutex_lock(&this->policy.mutex->_mutex);
	}

	MutexLockPolicy::UnLock::~UnLock()
	{
		pthread_mutex_lock(&this->lock.policy.mutex->_mutex);
	}

	MutexLockPolicy::UnLock::UnLock(MutexLockPolicy::Lock& lock) : lock(lock)
	{
		pthread_mutex_unlock(&this->lock.policy.mutex->_mutex);
	}
} // namespace ZipDirFs
