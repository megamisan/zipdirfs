/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_UTILITIES_LIST_CLEANUP_H
#define ZIPDIRFS_UTILITIES_LIST_CLEANUP_H

#include "ZipDirFs/Containers/Helpers/EntryListProxy.h"
#include "ZipDirFs/Containers/Helpers/Factory.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Utilities
{
	void CleanupEntryList(::ZipDirFs::Containers::Helpers::EntryListProxy&, ::ZipDirFs::Containers::Helpers::Factory&);
} // namespace ZipDirFs::Utilities

#endif // ZIPDIRFS_UTILITIES_LIST_CLEANUP_H
