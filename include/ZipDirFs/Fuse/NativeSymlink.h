/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_FUSE_NATIVESYMLINK_H
#define ZIPDIRFS_FUSE_NATIVESYMLINK_H

#include "ZipDirFs/Components/NativeChanged.h"
#include "ZipDirFs/Fuse/NativeTimePolicy.h"
#include "ZipDirFs/Utilities/DefaultPermission.h"
#include <boost/filesystem.hpp>
#include <fusekit/basic_symlink.h>
#include <fusekit/no_xattr.h>
#include <memory>

namespace boost::filesystem
{
	struct path;
}

namespace ZipDirFs::Fuse
{
	using namespace fusekit;

	struct NativeSymlink
		: public basic_symlink<NativeTimePolicy, Utilities::DefaultLinkPermission, no_xattr>
	{
		NativeSymlink(const boost::filesystem::path& path);
		std::time_t getChangeTime() const;
		std::time_t getModificationTime() const;

	private:
		const boost::filesystem::path path;
		std::unique_ptr<std::string> target;
		Components::NativeChanged changed;
	};
} // namespace ZipDirFs::Fuse

#endif // ZIPDIRFS_FUSE_NATIVESYMLINK_H
