/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIP_ENTRY_H
#define ZIPDIRFS_ZIP_ENTRY_H

#include "ZipDirFs/Zip/Base/Content.h"
#include "ZipDirFs/Zip/Base/Lib.h"
#include "ZipDirFs/Zip/Base/Stat.h"
#include <bitset>
#include <memory>
#include <string>

namespace ZipDirFs::Zip
{
	struct Stream;
	class Entry
	{
	public:
		Entry(const std::shared_ptr<Base::Lib>&, const std::string&, bool);
		~Entry();
		const Base::Stat& stat();
		bool isDir() const;

	protected:
		std::shared_ptr<Base::Content> open();
		const std::shared_ptr<Base::Lib> data;
		const std::string name;
		std::bitset<2> flags;
		Base::Stat cachedStat;
		Base::Content content;
		friend class Stream;
	};

} // namespace ZipDirFs::Zip

#endif // ZIPDIRFS_ZIP_ENTRY_H
