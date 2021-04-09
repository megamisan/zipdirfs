/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/EntryFactory.h"
#include "ZipDirFs/entry_definitions.h"
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <zip.h>

namespace ZipDirFs
{
	EntryFactory::EntryFactory()
	{
	}

	EntryFactory::~EntryFactory()
	{
	}

	fusekit::entry* createLink(const std::string& target)
	{
		wrapper_link* entry = new wrapper_link(target.c_str());
		return entry;
	}

	fusekit::entry* createDirectory(const std::string& realPath)
	{
		system_directory* entry = new system_directory();
		entry->setRealPath(realPath);
		return entry;
	}

	bool isZipFile(const std::string& file)
	{
		int handle = ::open(file.c_str(), O_RDONLY);

		if (handle < 0)
		{
			::perror("(EntryFactory)isZipFile: open");
			return false;
		}

		int error;
		::zip* zipFile = ::zip_fdopen(handle, 0, &error);

		if (zipFile == NULL)
		{
			::close(handle);
			return false;
		}

		::zip_close(zipFile);
		return true;
	}

	fusekit::entry* createZipRootDirectory(const std::string& zipFile)
	{
		zip_root_directory* entry = new zip_root_directory();
		entry->setZipFile(zipFile.c_str());
		return entry;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	fusekit::entry* EntryFactory::newEntry(const dirent* dirEntry, const std::string& basePath)
	{
		std::string realPath = basePath;
		realPath += '/';
		realPath += dirEntry->d_name;
		struct ::stat fileinfo;
		int res = ::stat(realPath.c_str(), &fileinfo);

		if (!res)
		{
			if (S_ISDIR(fileinfo.st_mode))
			{
				return createDirectory(realPath);
			}

			if (S_ISREG(fileinfo.st_mode) && isZipFile(realPath))
			{
				return createZipRootDirectory(realPath);
			}
		}

		return createLink(realPath);
	}
} // namespace ZipDirFs
