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
 * \\$
 */
#ifndef LINKTIME_H
#define LINKTIME_H

#include <time.h>

namespace ZipDirFs
{
	/**
	 * \brief Provides all the dates for all links.
	 */
	extern const ::time_t linksTime;

	/**
	 * \brief Represents a time definition for a link.
	 * This class must be used as the %Time template parameter to the \ref fusekit::basic_entry template class.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template <class Derived>
	class LinkTime
	{
	public:
		LinkTime() {}
		virtual ~LinkTime() {}
		::time_t modification_time()
		{
			return linksTime;
		}
		::time_t change_time()
		{
			return linksTime;
		}
		::time_t access_time()
		{
			return linksTime;
		}
		void update (int) {}
	protected:
	private:
	};
}

#endif // LINKTIME_H
