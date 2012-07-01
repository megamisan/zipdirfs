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
#include "EntryFactory.h"
#include "entry_definitions.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

EntryFactory::EntryFactory()
{
	//ctor
}

EntryFactory::~EntryFactory()
{
	//dtor
}

fusekit::entry* createLink(const std::string& target)
{
	wrapper_link *entry = new wrapper_link();
	entry->setLink(target.c_str());
	return entry;
}

fusekit::entry* createDirectory(const std::string& realPath)
{
	system_directory *entry = new system_directory();
	entry->setRealPath(realPath.c_str());
	return entry;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
fusekit::entry* EntryFactory::newEntry(const dirent* dirEntry, const std::string &basePath)
{
	std::string realPath = basePath;
	realPath += '/';
	realPath += dirEntry->d_name;
	struct stat fileinfo;
	int res = stat(realPath.c_str(), &fileinfo);
	if (!res)
	{
		if (S_ISDIR(fileinfo.st_mode))
		{
			return createDirectory(realPath);
		}
		if (S_ISREG(fileinfo.st_mode))
		{
			// TODO: Zip file case.
		}
	}
	return createLink(realPath);
}