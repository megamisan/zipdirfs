/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Fuse/NativeSymlink.h"
#include "ZipDirFs/Components/NativeChanged.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Fuse
{
	std::map<const boost::filesystem::path*, std::unique_ptr<std::string>> nativeSymlinkTargets;
	const char* NativeSymlinkInitTarget(const boost::filesystem::path&);
	std::unique_ptr<std::string> NativeSymlinkOwnTarget(const boost::filesystem::path&);

	NativeSymlink::NativeSymlink(const boost::filesystem::path& p) :
		basic_symlink(NativeSymlinkInitTarget(p)), path(p), changed(path), target(NativeSymlinkOwnTarget(p))
	{
		changed();
	}
	std::time_t NativeSymlink::getChangeTime() const { return changed; }
	std::time_t NativeSymlink::getModificationTime() const { return changed; }

	const char* NativeSymlinkInitTarget(const boost::filesystem::path& p)
	{
		nativeSymlinkTargets.emplace(&p, std::unique_ptr<std::string>(new std::string(p.native())));
		return nativeSymlinkTargets[&p].get()->c_str();
	}

	std::unique_ptr<std::string> NativeSymlinkOwnTarget(const boost::filesystem::path& p)
	{
		auto temp = std::move(nativeSymlinkTargets[&p]);
		nativeSymlinkTargets.erase(&p);
		return temp;
	}
} // namespace ZipDirFs::Fuse
