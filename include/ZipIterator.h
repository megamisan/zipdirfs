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
 * $
 */
#ifndef ZIPITERATOR_H
#define ZIPITERATOR_H

#include <stdint.h>
#include <iterator>
#include <time.h>
#include <string>

class ZipFile;
class ZipIterator;
struct zip;

struct ZipEntryFileInfo
{
	uint64_t index;
	std::string name;
	uint64_t size;
	time_t mtime;
private:
	ZipEntryFileInfo() {}
	friend class ZipIterator;
};

class ZipIterator : public std::iterator<std::input_iterator_tag, const ZipEntryFileInfo>
{
	public:
		virtual ~ZipIterator();
		ZipIterator(const ZipIterator &it);
		ZipIterator& operator =(const ZipIterator& it);
		ZipIterator& operator ++();
		inline ZipIterator operator ++(int) { ZipIterator it(*this); operator ++(); return it; }
		bool operator == (const ZipIterator &it) const;
		bool operator != (const ZipIterator &it) const;
		reference operator * () const;
		pointer operator -> () const;
		friend class ZipFile;
	protected:
	private:
		ZipFile *file;
		uint64_t position;
		uint64_t count;
		ZipEntryFileInfo fileinfo;
		ZipIterator(ZipFile *zipFile, bool end);
		void advance();
};

#endif // ZIPITERATOR_H
