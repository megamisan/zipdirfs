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
#ifndef ZIPFILEBUFFER_H
#define ZIPFILEBUFFER_H

#include "zipdirfs/ZipEntry.h"
#include <stdint.h>
#include <asm-generic/errno-base.h>
#include <fuse.h>
#include <time.h>

namespace zipdirfs
{
	template <class Derived>
	class ZipFileBuffer
	{
	public:
		ZipFileBuffer() : source (NULL) {}
		virtual ~ZipFileBuffer()
		{
			if (source != NULL)
			{
				delete source;
			}
		}
		int open (::fuse_file_info& fi)
		{
			return -EACCES;
		}
		int close (::fuse_file_info& fi)
		{
			return -EBADF;
		}
		int read (char* buf, ::size_t size, ::off_t offset, ::fuse_file_info& fi)
		{
			return -EBADF;
		}
		int write (const char* buf, ::size_t size, ::off_t offset, ::fuse_file_info& fi)
		{
			return -EACCES;
		}
		::uint64_t size()
		{
			return this->source->getSize();
		}
		int flush (::fuse_file_info& fi)
		{
			return -EBADF;
		}
		int truncate (::off_t offset)
		{
			return -EACCES;
		}
		::time_t getMTime()
		{
			return this->source->getMTime();
		}
		friend class ZipWalker;
	protected:
	private:
		ZipEntry* source;
		void setFileInfo (ZipEntry* source)
		{
			this->source = source;
		}

	};
}

#endif // ZIPFILEBUFFER_H
