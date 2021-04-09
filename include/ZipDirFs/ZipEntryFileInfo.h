/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIPENTRYFILEINFO_H
#define ZIPDIRFS_ZIPENTRYFILEINFO_H
#include <cstdint>
#include <string>
#include <ctime>

namespace ZipDirFs
{
	class ZipIterator;

	/**
	 * \brief Minimal representation of an entry in a \ref ZipFile.
	 * This class can only be instanciated by \ref ZipIterator.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	struct ZipEntryFileInfo
	{
		::uint64_t index;
		std::string name;
		::uint64_t size;
		::time_t mtime;
	private:
		ZipEntryFileInfo();
		friend class ZipIterator;
	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_ZIPENTRYFILEINFO_H
