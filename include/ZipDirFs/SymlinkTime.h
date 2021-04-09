/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_SYMLINKTIME_H
#define ZIPDIRFS_SYMLINKTIME_H

#include <ctime>

namespace ZipDirFs
{
	/**
	 * \brief Provides all the dates for all links.
	 */
	extern const timespec symlinksTime;

	/**
	 * \brief Represents a time definition for a link.
	 * This class must be used as the %Time template parameter to the \ref fusekit::basic_entry template class.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template <class Derived>
	class SymlinkTime
	{
	public:
		SymlinkTime() {}
		virtual ~SymlinkTime() {}
		timespec modification_time()
		{
			return symlinksTime;
		}
		timespec change_time()
		{
			return symlinksTime;
		}
		timespec access_time()
		{
			return symlinksTime;
		}
		void update(int) {}
	protected:
	private:
	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_SYMLINKTIME_H
