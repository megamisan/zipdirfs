/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIPWALKER_H
#define ZIPDIRFS_ZIPWALKER_H

#include "ZipDirFs/ZipIterator.h"
#include <string>
#include <iterator>
#include <utility>

namespace fusekit
{
	class entry;
} // namespace fusekit

namespace ZipDirFs
{
	class ZipFile;

	class ZipWalker : public std::iterator<std::input_iterator_tag, std::pair<std::string, fusekit::entry*> const >
	{
	public:
		ZipWalker(ZipFile* zipFile, std::string filterPath, bool end);
		ZipWalker(const ZipWalker& it);
		ZipWalker& operator = (const ZipWalker& it);
		ZipWalker& operator ++();
		inline ZipWalker operator ++ (int)
		{
			ZipWalker it(*this);
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
} // namespace ZipDirFs

#endif // ZIPDIRFS_ZIPWALKER_H
