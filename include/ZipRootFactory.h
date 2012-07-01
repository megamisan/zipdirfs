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
#ifndef ZIPROOTACTORY_H
#define ZIPROOTFACTORY_H

#include "NameSearchTree.h"
#include <fusekit/entry.h>
#include <fusekit/no_lock.h>
#include <time.h>

template <class LockingPolicy = fusekit::no_lock>
class ZipRootFactory : public LockingPolicy
{
	typedef typename ZipRootFactory<LockingPolicy>::lock lock;
	typedef NameSearchTree<fusekit::entry*, true> tree;
	public:
		ZipRootFactory() {}
		virtual ~ZipRootFactory() {}
		void setZipFile(const char* path)
		{
			if (!zipFile.empty())
			{
				return;
			}
			char *absolute = realpath(path, NULL);
			if (absolute != NULL)
			{
				zipFile = absolute;
				free(absolute);
			}
		}
		std::string getZipFile() const { return this->zipFile; }
		fusekit::entry *find(const char *name) { this->checkFile(); lock guard(*this); try { return entries.get(name); } catch (NotFoundException) { return NULL; } }
		int size() { this->checkFile(); lock guard(*this); return entries.size(); } //TODO: Count only directories. Internal value.
		int readdir(void *buf, fuse_fill_dir_t filler, off_t offset, fuse_file_info &)
		{
			this->checkFile();
			lock guard(*this);
			for (tree::iterator it = entries.begin(); it != entries.end(); it++)
			{
				filler(buf, it->c_str(), NULL, offset);
			}
			return 0;
		}
		inline time_t getLastUpdate() { return this->lastUpdate; }
	protected:
	private:
		tree entries;
		std::string zipFile;
		time_t lastUpdate;
		void checkFile()
		{
		}
};

#endif // ZIPROOTFACTORY_H
