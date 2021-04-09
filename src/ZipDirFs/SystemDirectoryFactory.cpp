/*
 * Copyright © 2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */

#include <climits>
#include <pthread.h>
#include <cstdlib>
#include <string>

namespace ZipDirFs
{
	struct RealpathHelper
	{
	    pthread_mutex_t mutex;
		char buffer[PATH_MAX + 1];
		RealpathHelper();
		~RealpathHelper();
	} realpath_helper;

	struct RealpathLocker
	{
	    RealpathLocker();
	    ~RealpathLocker();
	};

	bool realpath(const std::string& path, std::string& resolved_path)
	{
	    RealpathLocker lock;
		const char* result = ::realpath(path.c_str(), realpath_helper.buffer);
		if (result != NULL)
		{
			resolved_path = result;
		}
		return result != NULL;
	}

	RealpathHelper::RealpathHelper()
	{
        pthread_mutex_init(&mutex, NULL);
	}

	RealpathHelper::~RealpathHelper()
	{
	    pthread_mutex_destroy(&mutex);
	}


	RealpathLocker::RealpathLocker()
	{
	    pthread_mutex_lock(&realpath_helper.mutex);
	}

	RealpathLocker::~RealpathLocker()
	{
	    pthread_mutex_unlock(&realpath_helper.mutex);
	}
} // namespace ZipDirFs
