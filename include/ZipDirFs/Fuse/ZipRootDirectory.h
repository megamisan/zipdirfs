/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_FUSE_ZIPROOTDIRECTORY_H
#define ZIPDIRFS_FUSE_ZIPROOTDIRECTORY_H

#include "ZipDirFs/Components/ChangedProxy.h"
#include "ZipDirFs/Components/EntryListProxyProxy.h"
#include "ZipDirFs/Fuse/DirectoryNode.h"
#include "ZipDirFs/Fuse/NativeTimePolicy.h"
#include "ZipDirFs/Utilities/DefaultPermission.h"
#include <boost/filesystem.hpp>
#include <fusekit/basic_directory.h>
#include <fusekit/no_xattr.h>

namespace ZipDirFs::Fuse
{
	using namespace fusekit;

	struct ZipRootDirectory : public basic_directory<DefaultDirectoryNode, NativeTimePolicy,
								  Utilities::DefaultDirectoryPermission, no_xattr>
	{
		ZipRootDirectory(const boost::filesystem::path& path);
		std::time_t getChangeTime() const;
		std::time_t getModificationTime() const;
		EntryGenerator& generator();
		EntryGenerator::proxy_ptr& proxy();

	private:
		const boost::filesystem::path path;
		std::shared_ptr<::ZipDirFs::Components::ChangedProxy> _changed;
		EntryGenerator::factory_ptr _factory;
		EntryGenerator::proxy_ptr _proxyBase;
		EntryGenerator::proxy_ptr _proxy;
		EntryGenerator _generator;
	};
} // namespace ZipDirFs::Fuse

#endif // ZIPDIRFS_FUSE_ZIPROOTDIRECTORY_H
