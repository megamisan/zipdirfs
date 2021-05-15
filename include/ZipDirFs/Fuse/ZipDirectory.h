/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_FUSE_ZIPDIRECTORY_H
#define ZIPDIRFS_FUSE_ZIPDIRECTORY_H

#include "ZipDirFs/Fuse/DirectoryNode.h"
#include "ZipDirFs/Fuse/ZipTimePolicy.h"
#include "ZipDirFs/Utilities/DefaultPermission.h"
#include <boost/filesystem.hpp>
#include <fusekit/basic_directory.h>
#include <fusekit/no_xattr.h>

namespace ZipDirFs::Fuse
{
	using namespace fusekit;

	struct ZipDirectory : public basic_directory<DefaultDirectoryNode, ZipTimePolicy,
							  Utilities::DefaultDirectoryPermission, no_xattr>
	{
		ZipDirectory(const boost::filesystem::path& zip, const std::string& folder,
			EntryGenerator::changed_ptr& rootChanged);
		std::time_t getParentModificationTime() const;
		std::time_t getModificationTime() const;
		EntryGenerator& generator();
		EntryGenerator::proxy_ptr& proxy();

	private:
		const boost::filesystem::path zip;
		const std::string folder;
		EntryGenerator::proxy_ptr _proxy;
		EntryGenerator::changed_ptr _changed;
		EntryGenerator::changed_ptr _rootChanged;
		EntryGenerator _generator;
	};
} // namespace ZipDirFs::Fuse

#endif // ZIPDIRFS_FUSE_ZIPDIRECTORY_H
