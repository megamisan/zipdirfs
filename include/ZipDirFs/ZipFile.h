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
#ifndef ZIPFILE_H
#define ZIPFILE_H

#include "ZipDirFs/ZipIterator.h"
#include "ZipDirFs/MutexLockPolicy.h"
#include <string>

struct zip;
namespace ZipDirFs
{
	class ZipEntry;

	/**
	 * \brief Represents a zip file.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class ZipFile : MutexLockPolicy
	{
	public:
		ZipFile(const std::string& path);
		virtual ~ZipFile();
		ZipIterator begin();
		ZipIterator end();
		ZipEntry* getEntry(const ZipEntryFileInfo& fileinfo);
		const std::string& getFilePath() const;
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
