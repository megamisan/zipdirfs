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
#include "zipdirfs/ZipWalker.h"
#include "zipdirfs/ZipFile.h"
#include "zipdirfs/entry_definitions.h"
#include <string.h>

namespace zipdirfs
{
	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	void ZipWalker::advance()
	{
		this->advanceInit();
		this->advanceToNext();
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	void ZipWalker::advanceInit()
	{
		if (begin != end)
		{
			this->entry.first = "";
			this->entry.second = NULL;
			begin++;
		}
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	void ZipWalker::advanceToNext()
	{
		while ( begin != end )
		{
			if ( (this->filterPath.size() == 0) || (::strncmp (begin->name.c_str(), this->filterPath.c_str(), this->filterPath.size() ) == 0) )
			{
				std::string name (begin->name.substr (this->filterPath.size() ) );

				if (name.size() > 0)
				{
					::size_t pos = name.find ('/');

					if ( (pos == std::string::npos) || (pos + 1 == name.size() ) )
					{
						this->entry.first = name;

						if (pos != std::string::npos)
						{
							this->entry.first.erase (pos);
							zip_directory* dir = new zip_directory();
							dir->setDirectoryInfo (zipFile, begin->name, begin->mtime);
							this->entry.second = dir;
						}
						else
						{
							zip_file* file = new zip_file();
							file->setFileInfo (this->zipFile->getEntry (*begin) );
							this->entry.second = file;
						}
					}
				}
			}

			if (this->entry.second != NULL)
			{
				break;
			}

			begin++;
		}
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipWalker::~ZipWalker()
	{
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipWalker::pointer ZipWalker::operator->() const
	{
		return &this->entry;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipWalker::reference ZipWalker::operator*() const
	{
		return this->entry;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	bool ZipWalker::operator!= (const ZipWalker& it) const
	{
		return (this->begin != it.begin) || (this->end != it.end) || (this->filterPath != it.filterPath);
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	bool ZipWalker::operator== (const ZipWalker& it) const
	{
		return (this->begin == it.begin) || (this->end == it.end) || (this->filterPath == it.filterPath);
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipWalker& ZipWalker::operator++()
	{
		this->advance();
		return *this;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipWalker& ZipWalker::operator= (const ZipWalker& it)
	{
		this->zipFile = it.zipFile;
		this->begin = it.begin;
		this->end = it.end;
		this->filterPath = it.filterPath;
		this->entry = it.entry;
		return *this;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipWalker::ZipWalker (const ZipWalker& it) : zipFile (it.zipFile), begin (it.begin), end (it.end), filterPath (it.filterPath), entry (it.entry)
	{
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipWalker::ZipWalker (ZipFile* zipFile, std::string filterPath, bool end) : zipFile (zipFile), begin (end ? zipFile->end() : zipFile->begin() ), end (zipFile->end() ), filterPath (filterPath)
	{
		this->entry.second = NULL;

		if (!end)
		{
			this->advanceToNext();
		}
	}
}
