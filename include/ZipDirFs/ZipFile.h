/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIPFILE_H
#define ZIPDIRFS_ZIPFILE_H

#include "ZipDirFs/ZipIterator.h"
#include "ZipDirFs/MutexLockPolicy.h"
#include <string>

struct zip;
namespace ZipDirFs
{
	class ZipEntry;

	/**
	 * \brief Represents a zip file.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class ZipFile : MutexLockPolicy
	{
	public:
		ZipFile(const std::string& path);
		virtual ~ZipFile();
		ZipIterator begin();
		ZipIterator end();
		ZipEntry* getEntry(const ZipEntryFileInfo& fileinfo);
		const std::string& getFilePath() const;
		friend class ZipIterator;
		friend class ZipEntry;
	protected:
	private:
		const std::string path;
		::zip* zipFile;
		::zip* getZip();
		void releaseZip();
		int volatile refCount;
	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_ZIPFILE_H
