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

#include "ZipDirFs/MutexLockPolicy.h"
#include <sys/types.h>
#include <time.h>
#include <stdint.h>

struct zip_file;
namespace ZipDirFs
{
	class ZipFile;
	struct ZipEntryFileInfo;

	/**
	 * \brief Represents a file in a ZipFile.
	 * This class can only be instanciated by \ref ZipFile.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class ZipEntry : MutexLockPolicy
	{
	public:
		virtual ~ZipEntry();
		/**
		 * \brief Open the file for reading.
		 */
		bool open();
		/**
		 * \brief Release the file.
		 * Resources are freed when this method has been called the same number of times as \ref open().
		 */
		bool release();
		/**
		 * \brief Reads a portion of the file.
		 * \param buf The buffer where to write the data.
		 * \param size The number of bytes to read.
		 * \param offset The position of the first byte to read.
		 * \return The number of bytes read or -EIO on error.
		 */
		int read (char* buf, ::size_t size, ::off_t offset);
		/**
		 * \brief Retrieves the size of the file.
		 * \return The size of the file.
		 */
		const ::uint64_t getSize() const
		{
			return this->size;
		}
		/**
		 * \brief Retrieves the modification time of the file.
		 * \return The modification time.
		 */
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
		/**
		 * \brief Initializes the entry.
		 * Initialize using the source ZipFile and the information about the entry.
		 * \param file The ZipFile from which the entry comes from.
		 * \param entryInfo Information about the entry: index, modification time and size.
		 */
		ZipEntry (ZipFile &file, const ZipEntryFileInfo& entryInfo);
		/**
		 * \brief Fills in the buffer to the requested position.
		 * If an error occured during decompression, it fails.
		 * \param position The last position that must be read +1.
		 * \return `true` on success, `false` on failure.
		 */
		bool ensureRead(::uint64_t position);
	};
}

#endif // ZIPENTRY_H
