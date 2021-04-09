/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIPITERATOR_H
#define ZIPDIRFS_ZIPITERATOR_H

#include "ZipDirFs/ZipEntryFileInfo.h"
#include <cstdint>
#include <iterator>
#include <string>

namespace ZipDirFs
{
	class ZipFile;

	class ZipIterator : public std::iterator<std::input_iterator_tag, const ZipEntryFileInfo>
	{
	public:
		virtual ~ZipIterator();
		ZipIterator(const ZipIterator& it);
		ZipIterator& operator = (const ZipIterator& it);
		ZipIterator& operator ++();
		inline ZipIterator operator ++ (int)
		{
			ZipIterator it(*this);
			operator ++();
			return it;
		}
		bool operator == (const ZipIterator& it) const;
		bool operator != (const ZipIterator& it) const;
		reference operator * () const;
		pointer operator -> () const;
		friend class ZipFile;
	protected:
	private:
		ZipFile* file;
		::uint64_t position;
		::uint64_t count;
		ZipEntryFileInfo fileinfo;
		ZipIterator(ZipFile* zipFile, bool end);
		void advance();
	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_ZIPITERATOR_H
