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
#ifndef ZIPENTRY_H
#define ZIPENTRY_H

#include <sys/types.h>
#include <time.h>
#include <stdint.h>

struct zip_file;
namespace zipdirfs
{
	class ZipFile;
	struct ZipEntryFileInfo;

	class ZipEntry
	{
	public:
		virtual ~ZipEntry();
		bool open();
		bool release();
		int read (char* buf, ::size_t size, ::off_t offset);
		const ::uint64_t getSize() const
		{
			return this->size;
		}
		const ::time_t getMTime() const
		{
			return this->mtime;
		}
		friend class ZipFile;
	protected:
	private:
		ZipFile& file;
		const ::uint64_t index;
		const ::uint64_t size;
		const ::time_t mtime;
		char *buffer;
		::uint64_t progress;
		int refCount;
		::zip_file *zipFileEntry;
		ZipEntry (ZipFile &, const ZipEntryFileInfo&);
		bool ensureRead(::uint64_t position);
	};
}

#endif // ZIPENTRY_H
