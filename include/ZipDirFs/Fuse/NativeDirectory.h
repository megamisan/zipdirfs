/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_FUSE_NATIVEDIRECTORY_H
#define ZIPDIRFS_FUSE_NATIVEDIRECTORY_H

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

	/**
	 * @brief A directory entry representing a real filesystem directory
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	struct NativeDirectory : public basic_directory<DefaultDirectoryNode, NativeTimePolicy,
								 Utilities::DefaultDirectoryPermission, no_xattr>
	{
		NativeDirectory(const boost::filesystem::path& path);
		~NativeDirectory();
		std::time_t getChangeTime() const;
		std::time_t getModificationTime() const;
		EntryGenerator& generator();
		EntryGenerator::proxy_ptr& proxy();
		EntryGenerator::locker_ptr& locker();

	private:
		const boost::filesystem::path path;
		std::shared_ptr<::ZipDirFs::Components::ChangedProxy> _changed;
		EntryGenerator::factory_ptr _factory;
		EntryGenerator::proxy_ptr _proxyBase;
		EntryGenerator::proxy_ptr _proxy;
		EntryGenerator::locker_ptr _locker;
		EntryGenerator _generator;
	};
} // namespace ZipDirFs::Fuse

#endif // ZIPDIRFS_FUSE_NATIVEDIRECTORY_H
