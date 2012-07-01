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
#ifndef ZIPENTRY_H
#define ZIPENTRY_H

#include <time.h>
#include <stdint.h>

class ZipFile;
struct ZipEntryFileInfo;

class ZipEntry
{
	public:
		virtual ~ZipEntry();
		bool open();
		bool release();
		const uint64_t getSize() const { return this->size; }
		const time_t getMTime() const { return this->mtime; }
		friend class ZipFile;
	protected:
	private:
		ZipFile &file;
		const uint64_t index;
		const uint64_t size;
		const time_t mtime;
		ZipEntry(ZipFile &, const ZipEntryFileInfo&);
};

#endif // ZIPENTRY_H
