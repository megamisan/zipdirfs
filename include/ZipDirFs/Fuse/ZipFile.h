/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_FUSE_NATIVESYMLINK_H
#define ZIPDIRFS_FUSE_NATIVESYMLINK_H

#include "ZipDirFs/Components/ZipFileChanged.h"
#include "ZipDirFs/Fuse/ZipFileBuffer.h"
#include "ZipDirFs/Fuse/ZipTimePolicy.h"
#include "ZipDirFs/Utilities/DefaultPermission.h"
#include <boost/filesystem.hpp>
#include <fusekit/basic_file.h>
#include <fusekit/no_xattr.h>
#include <memory>

namespace ZipDirFs::Fuse
{
	using namespace fusekit;

	/**
	 * @brief A file entry representing a file in a zip archive
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	struct ZipFile : public basic_file<DefaultZipFileBuffer, ZipTimePolicy,
						 Utilities::DefaultFilePermission, no_xattr>
	{
		ZipFile(const boost::filesystem::path& zip, const std::string& file,
			std::shared_ptr<::ZipDirFs::Containers::Helpers::Changed>& rootChanged);
		std::time_t getParentModificationTime() const;
		std::time_t getModificationTime() const;
		std::shared_ptr<::ZipDirFs::Zip::Entry> entry() const;
		off_t entrySize() const;

	private:
		const boost::filesystem::path zip;
		const std::string file;
		Components::ZipFileChanged changed;
		std::shared_ptr<::ZipDirFs::Containers::Helpers::Changed> _rootChanged;
		const off_t size;
	};
} // namespace ZipDirFs::Fuse

#endif // ZIPDIRFS_FUSE_NATIVESYMLINK_H
