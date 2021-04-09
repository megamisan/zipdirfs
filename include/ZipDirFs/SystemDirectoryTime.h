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
#ifndef SYSTEMDIRECTORYTIME_H
#define SYSTEMDIRECTORYTIME_H

#include <time.h>

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
}

#endif // SYSTEMDIRECTORYTIME_H
