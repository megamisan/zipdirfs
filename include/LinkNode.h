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
#ifndef LINKNODE_H
#define LINKNODE_H

#include <fusekit/entry.h>
#include <string.h>

template <
class Factory,
class Derived
>
class LinkNode : public Factory
{
public:
	LinkNode() {}
	virtual ~LinkNode() {}
	fusekit::entry* find (const char *)
	{
		return 0;
	}
	int target (char* path, size_t size)
	{
		path[0] = 0;
		path[size - 1] = 0;
		strncat (path, factory().getLink().c_str(), size - 1);
		return 0;
	}
	int links()
	{
		return 1;
	}
	int readdir (void *, fuse_fill_dir_t filler, off_t offset, fuse_file_info& finfo)
	{
		return -ENOTDIR;
	}
	int mknod (const char* name, mode_t mode, dev_t type)
	{
		return -ENOTDIR;
	}
	int unlink (const char* name)
	{
		return -ENOTDIR;
	}
	int mkdir (const char* name, mode_t mode)
	{
		return -ENOTDIR;
	}
	int rmdir (const char* name)
	{
		return -ENOTDIR;
	}
protected:
private:
	inline Factory& factory()
	{
		return static_cast<Factory&> (*this);
	}
};

#endif // LINKNODE_H
