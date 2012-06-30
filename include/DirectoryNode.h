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
#ifndef DIRECTORYNODE_H
#define DIRECTORYNODE_H

#include <fusekit/entry.h>

template <
	class Factory,
	class Derived
>
class DirectoryNode : public Factory
{
	public:
		/** Default constructor */
		DirectoryNode();
		/** Default destructor */
		virtual ~DirectoryNode();
		entry *find(const char *name)
		{
			return factory().find(name);
		}
		int links()
		{
			return factory().size() + 2;
		}
		int readdir(void *buf, fuse_fill_dir_t filler, off_t offset, fuse_file_info &)
		{
			filler(buf, ".", NULL, offset);
			filler(buf, "..", NULL, offset);
			// TODO: factory's enumeration.
			return 0;
		}
		int mknod(const char *name, mode_t mode, dev_t type)
		{
			return -EACCES;
		}
		int unlink(const char *name)
		{
			return -EACCES;
		}
		int mkdir(const char *name, mode_t mode)
		{
			return -EACCES;
		}
		int rmdir(const char *name)
		{
			return -EACCES;
		}
	protected:
	private:
		inline Factory &factory()
		{
			return static_cast<Factory&>(*this);
		}
};

#endif // DIRECTORYNODE_H
