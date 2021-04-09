/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ENTRYFACTORY_H
#define ZIPDIRFS_ENTRYFACTORY_H

#include <fusekit/entry.h>
#include <dirent.h>
#include <string>

namespace ZipDirFs
{
	/**
	 * \brief Helps create new \ref fusekit::entry.
	 * This class is not instanciable.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class EntryFactory
	{
	public:
		virtual ~EntryFactory();
		/**
		 * \brief Retrieves a new \ref fusekit::entry.
		 * Identify the kind of entry to create depending on the real entry kind and content.
		 * \param dirEntry An instance of \ref dirent filled by \ref readdir or \ref readdir_r.
		 * \param basePath The full path on the file system of the real entry.
		 * \return The newly created entry.
		 */
		static fusekit::entry* newEntry(const ::dirent* dirEntry, const std::string& basePath);
	protected:
	private:
		EntryFactory();
	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_ENTRYFACTORY_H
