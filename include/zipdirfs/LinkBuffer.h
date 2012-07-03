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
 * $
 */
#ifndef LINKBUFFER_H
#define LINKBUFFER_H

#include <fusekit/entry.h>

namespace zipdirfs
{
	template <class Derived>
	class LinkBuffer
	{
	public:
		LinkBuffer() {}
		virtual ~LinkBuffer() {}
		int open (::fuse_file_info&)
		{
			return -EINVAL;
		}
		int close (::fuse_file_info&)
		{
			return -EINVAL;
		}
		int size()
		{
			return static_cast<Derived*> (this)->getLink().size();
		}
		int read (char*, ::size_t, ::off_t, ::fuse_file_info&)
		{
			return -EINVAL;
		}
		int write (const char*, ::size_t, ::off_t, ::fuse_file_info&)
		{
			return -EINVAL;
		}
		int flush (::fuse_file_info&)
		{
			return -EINVAL;
		}
		int truncate (::off_t offset)
		{
			return -EINVAL;
		}
	protected:
	private:
	};
}

#endif // LINKBUFFER_H
