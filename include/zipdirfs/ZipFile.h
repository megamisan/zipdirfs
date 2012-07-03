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
#ifndef ZIPFILE_H
#define ZIPFILE_H

#include "zipdirfs/ZipIterator.h"
#include <string>

struct zip;
namespace zipdirfs
{
	class ZipEntry;

	class ZipFile
	{
	public:
		ZipFile (const std::string& path);
		virtual ~ZipFile();
		ZipIterator begin();
		ZipIterator end();
		ZipEntry* getEntry (const ZipEntryFileInfo& fileinfo);
		friend class ZipIterator;
		friend class ZipEntry;
	protected:
	private:
		const std::string path;
		::zip* zipFile;
		::zip* getZip();
		void releaseZip();
		int volatile refCount;
	};
}

#endif // ZIPFILE_H
