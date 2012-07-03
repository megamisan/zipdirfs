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
#ifndef ZIPWALKER_H
#define ZIPWALKER_H

#include "zipdirfs/ZipIterator.h"
#include <string>
#include <iterator>
#include <utility>

namespace fusekit
{
	class entry;
}

namespace zipdirfs
{
	class ZipFile;

	class ZipWalker : public std::iterator<std::input_iterator_tag, std::pair<std::string, fusekit::entry*> const >
	{
	public:
		ZipWalker (ZipFile* zipFile, std::string filterPath, bool end);
		ZipWalker (const ZipWalker& it);
		ZipWalker& operator = (const ZipWalker& it);
		ZipWalker& operator ++();
		inline ZipWalker operator ++ (int)
		{
			ZipWalker it (*this);
			operator ++();
			return it;
		}
		bool operator == (const ZipWalker& it) const;
		bool operator != (const ZipWalker& it) const;
		reference operator * () const;
		pointer operator -> () const;
		virtual ~ZipWalker();
	protected:
	private:
		ZipFile* zipFile;
		ZipIterator begin;
		ZipIterator end;
		std::string filterPath;
		std::pair<std::string, fusekit::entry*> entry;
		void advance();
		void advanceInit();
		void advanceToNext();
	};
}

#endif // ZIPWALKER_H
