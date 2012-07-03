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
#include "zipdirfs/ZipIterator.h"
#include "zipdirfs/ZipFile.h"
#include <zip.h>

namespace zipdirfs
{
	ZipIterator::~ZipIterator()
	{
		this->file->releaseZip();
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipIterator::pointer ZipIterator::operator->() const
	{
		return &this->fileinfo;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipIterator::reference ZipIterator::operator*() const
	{
		return this->fileinfo;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	bool ZipIterator::operator!= (const ZipIterator& it) const
	{
		return (this->file != it.file) || (this->position != it.position);
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	bool ZipIterator::operator== (const ZipIterator& it) const
	{
		return (this->file == it.file) && (this->position == it.position);
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipIterator& ZipIterator::operator++()
	{
		this->advance();
		return *this;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipIterator& ZipIterator::operator= (const ZipIterator& it)
	{
		it.file->getZip();
		this->file->releaseZip();
		this->file = it.file;
		this->position = it.position;
		this->count = it.count;
		this->fileinfo = it.fileinfo;
		return *this;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipIterator::ZipIterator (const ZipIterator& it) : file (it.file), position (it.position), count (it.count), fileinfo (it.fileinfo)
	{
		this->file->getZip();
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipIterator::ZipIterator (ZipFile* zipFile, bool end) : file (zipFile)
	{
		this->count = ::zip_get_num_entries (zipFile->getZip(), 0);

		if (end)
		{
			this->position = this->count - 1;
		}
		else
		{
			this->position = -1;
		}

		this->advance();
	}

	void ZipIterator::advance()
	{
		this->position++;
		this->fileinfo.index = -1;
		this->fileinfo.mtime = 0;
		this->fileinfo.size = -1;
		this->fileinfo.name = "";
		struct ::zip_stat fileinfo;

		if (::zip_stat_index (this->file->getZip(), this->position, 0, &fileinfo) == 0)
		{
			if (fileinfo.valid & ZIP_STAT_INDEX)
			{
				this->fileinfo.index = fileinfo.index;
			}

			if (fileinfo.valid & ZIP_STAT_NAME)
			{
				this->fileinfo.name = fileinfo.name;
			}

			if (fileinfo.valid & ZIP_STAT_SIZE)
			{
				this->fileinfo.size = fileinfo.size;
			}

			if (fileinfo.valid & ZIP_STAT_MTIME)
			{
				this->fileinfo.mtime = fileinfo.mtime;
			}
		}

		this->file->releaseZip();
	}
}
