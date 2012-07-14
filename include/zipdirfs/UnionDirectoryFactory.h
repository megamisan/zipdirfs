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
#ifndef UNIONDIRECTORYFACTORY_H
#define UNIONDIRECTORYFACTORY_H

#include <fusekit/entry.h>
#include <fusekit/no_lock.h>
#include <time.h>

namespace zipdirfs
{
	/**
	 * \brief Represents a join between two directory factory.
	 * This class must be used as a template parameter of \brief DirectoryNode.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template < class Factory1, class Factory2, class LockPolicy = fusekit::no_lock >
	class UnionDirectoryFactory : public Factory1<LockPolicy>, public Factory2<LockPolicy>
	{
	public:
		UnionDirectoryFactory() {}
		virtual ~UnionDirectoryFactory() {}
	protected:
		fusekit::entry* find (const char* name)
		{
			fusekit::entry *entry = factory1().find(name);
			if (entry == NULL) entry = factory2().find(name);
			return entry;
		}
		int size()
		{
			return factory1().size() + factory2.size();
		}
		int links()
		{
			return factory1().links() + factory2.links();
		}
		int readdir (void* buf, ::fuse_fill_dir_t filler, ::off_t offset, ::fuse_file_info &finfo)
		{
			factory1().readdir(buf, filler, offset, finfo);
			factory2().readdir(buf, filler, offset, finfo);
			return 0;
		}
		inline ::time_t getLastUpdate()
		{
			::time_t u1 = factory1().getLastUpdate();
			::time_t u2 = factory2().getLastUpdate();
			return (u1 > u2) ? u1 : u2;
		}
	private:
		inline Factory1& factory1()
		{
			return static_cast<Factory1&> (*this);
		}
		inline Factory2& factory2()
		{
			return static_cast<Factory2&> (*this);
		}
	};
}

#endif // UNIONDIRECTORYFACTORY_H
