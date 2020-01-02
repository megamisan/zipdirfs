/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
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
 */
#include "ZipDirFs/EntryFactory.h"
#include "ZipDirFs/entry_definitions.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <zip.h>

namespace ZipDirFs
{
	EntryFactory::EntryFactory()
	{
	}

	EntryFactory::~EntryFactory()
	{
	}

	fusekit::entry* createLink (const std::string& target)
	{
		wrapper_link* entry = new wrapper_link(target.c_str());
		return entry;
	}

	fusekit::entry* createDirectory (const std::string& realPath)
	{
		system_directory* entry = new system_directory();
		entry->setRealPath (realPath.c_str() );
		return entry;
	}

	bool isZipFile (const std::string& file)
	{
		int handle = ::open (file.c_str(), O_RDONLY);

		if (handle < 0)
		{
			::perror ("(EntryFactory)isZipFile: open");
			return false;
		}

		int error;
		::zip* zipFile = ::zip_fdopen (handle, 0, &error);

		if (zipFile == NULL)
		{
			::close (handle);
			return false;
		}

		::zip_close (zipFile);
		return true;
	}

	fusekit::entry* createZipRootDirectory (const std::string& zipFile)
	{
		zip_root_directory* entry = new zip_root_directory();
		entry->setZipFile (zipFile.c_str() );
		return entry;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	fusekit::entry* EntryFactory::newEntry (const dirent* dirEntry, const std::string& basePath)
	{
		std::string realPath = basePath;
		realPath += '/';
		realPath += dirEntry->d_name;
		struct ::stat fileinfo;
		int res = ::stat (realPath.c_str(), &fileinfo);

		if (!res)
		{
			if (S_ISDIR (fileinfo.st_mode) )
			{
				return createDirectory (realPath);
			}

			if (S_ISREG (fileinfo.st_mode) && isZipFile (realPath) )
			{
				return createZipRootDirectory (realPath);
			}
		}

		return createLink (realPath);
	}
}
