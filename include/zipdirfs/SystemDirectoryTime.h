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
#ifndef SYSTEMDIRECTORYTIME_H
#define SYSTEMDIRECTORYTIME_H

#include <time.h>

namespace zipdirfs
{
	template <class Derived>
	class SystemDirectoryTime
	{
	public:
		SystemDirectoryTime() : create (::time (NULL) ) {}
		virtual ~SystemDirectoryTime() {}
		time_t modification_time()
		{
			return static_cast<Derived*> (this)->getLastUpdate();
		}
		time_t change_time()
		{
			return this->create;
		}
		time_t access_time()
		{
			return static_cast<Derived*> (this)->getLastUpdate();
		}
		void update (int) {}
	protected:
	private:
		::time_t create;
	};
}

#endif // SYSTEMDIRECTORYTIME_H
