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
#include "zipdirfs/ZipFile.h"
#include "zipdirfs/ZipEntry.h"
#include <iostream>
#include <fcntl.h>
#include <zip.h>
#include <errno.h>

namespace zipdirfs
{
	ZipFile::ZipFile (const std::string& path) : path (path), zipFile (NULL)
	{
	}

	ZipFile::~ZipFile()
	{
		if (this->refCount > 0)
		{
			std::cerr << "ZipFile::~ZipFile: zip instance is still open. " << this->refCount << " refs." << std::endl;
		}
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipEntry* ZipFile::getEntry (const ZipEntryFileInfo& fileinfo)
	{
		return new ZipEntry (*this, fileinfo);
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipIterator ZipFile::end()
	{
		return ZipIterator (this, true);
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipIterator ZipFile::begin()
	{
		return ZipIterator (this, false);
	}

	::zip* ZipFile::getZip()
	{
		if (this->refCount == 0)
		{
			int handle = ::open (path.c_str(), O_RDONLY | O_NOATIME | O_NOCTTY);

			if (handle < 0)
			{
				::perror ("ZipFile::getZip: open");
				return NULL;
			}

			int error;
			::zip* zipFile = ::zip_fdopen (handle, 0, &error);

			if (zipFile == NULL)
			{
				int len = ::zip_error_to_str (NULL, 0, error, errno);
				char* message = new char[len + 1];
				::zip_error_to_str (message, 1024, error, errno);
				std::cerr << "zip_fdopen: " << message << std::endl;
				delete[] message;
				::close (handle);
				return NULL;
			}

			if (this->refCount == 0)
			{
				this->refCount = 1;
				this->zipFile = zipFile;
			}
			else
			{
				this->refCount++;
				::zip_close (zipFile);
			}
		}
		else
		{
			this->refCount++;
		}

		return this->zipFile;
	}

	void ZipFile::releaseZip()
	{
		::zip* zipFile = this->zipFile;
		this->refCount--;

		if (this->refCount == 0)
		{
			::zip_close (zipFile);
		}
	}
}
