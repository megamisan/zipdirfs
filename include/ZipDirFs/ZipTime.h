/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIPTIME_H
#define ZIPDIRFS_ZIPTIME_H

#include <ctime>

namespace ZipDirFs
{
	template <class Derived>
	class ZipTime
	{
	public:
		ZipTime() {}
		virtual ~ZipTime() {}
		timespec modification_time()
		{
			return static_cast<Derived*>(this)->getMTime();
		}
		timespec change_time()
		{
			return static_cast<Derived*>(this)->getMTime();
		}
		timespec access_time()
		{
			return static_cast<Derived*>(this)->getMTime();
		}
		void update(int) {}
	protected:
	private:
	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_ZIPTIME_H
