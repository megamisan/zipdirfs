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
#include "zipdirfs/ZipEntry.h"
#include "zipdirfs/ZipFile.h"
#include "zipdirfs/ZipEntryFileInfo.h"
#include <zip.h>
#include <asm-generic/errno-base.h>
#include <string.h>

namespace zipdirfs
{
	ZipEntry::ZipEntry (ZipFile& file, const ZipEntryFileInfo& fileinfo) : file (file), index (fileinfo.index), size (fileinfo.size), mtime (fileinfo.mtime), buffer(NULL), progress(0), refCount(0), zipFileEntry(0)
	{
	}

	ZipEntry::~ZipEntry()
	{
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	bool ZipEntry::release()
	{
		this->refCount--;
		if (this->refCount == 0)
		{
			delete[] this->buffer;
			this->buffer = NULL;
			this->progress = 0;
			::zip_fclose(this->zipFileEntry);
			this->zipFileEntry = NULL;
			this->file.releaseZip();
		}
		return true;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	bool ZipEntry::open()
	{
		if (this->refCount == 0)
		{
			this->buffer = new char[this->size];
			if (this->buffer == NULL) return false;
			this->zipFileEntry = ::zip_fopen_index(this->file.getZip(), this->index, 0);
			if (this->zipFileEntry == NULL)
			{
				this->file.releaseZip();
				delete [] this->buffer;
				return false;
			}
			this->refCount = 1;
		}
		return true;
	}

	int ZipEntry::read (char* buf, ::size_t size, ::off_t offset)
	{
		if (offset + size >= (::off_t)this->size) size = (::off_t)this->size - offset;
		if (!this->ensureRead(size + offset)) return -EIO;
		memcpy(buf, this->buffer + offset, size);
		return size;
	}

	bool ZipEntry::ensureRead(::uint64_t position)
	{
		if (position < this->progress) return true;
		int read;
		while (this->progress < position)
		{
			read = ::zip_fread(this->zipFileEntry, this->buffer + this->progress, position - this->progress);
			if (read < 0) return false;
			this->progress += read;
		}
		return true;
	}
}
