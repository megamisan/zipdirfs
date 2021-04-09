/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_SYSTEMDIRECTORYTIME_H
#define ZIPDIRFS_SYSTEMDIRECTORYTIME_H

#include <ctime>

namespace ZipDirFs
{
	/**
	 * \brief Represents a time definition for a system directory or a system file transformed into a directory.
	 * This class must be used as the %Time template parameter to the \ref fusekit::basic_entry template class.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template <class Derived>
	class SystemDirectoryTime
	{
	public:
		SystemDirectoryTime()
		{
			clock_gettime(CLOCK_REALTIME, &create);
		}
		virtual ~SystemDirectoryTime() {}
		timespec modification_time()
		{
			return static_cast<Derived*>(this)->getLastUpdate();
		}
		timespec change_time()
		{
			return this->create;
		}
		timespec access_time()
		{
			return static_cast<Derived*>(this)->getLastUpdate();
		}
		void update(int) {}
	protected:
	private:
		struct timespec create;
	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_SYSTEMDIRECTORYTIME_H
