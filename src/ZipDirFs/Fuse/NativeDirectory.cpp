/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Fuse/NativeDirectory.h"
#include "ZipDirFs/Components/NativeChanged.h"
#include "ZipDirFs/Components/NativeDirectoryEnumerator.h"
#include "ZipDirFs/Components/NativeFactory.h"
#include "ZipDirFs/Containers/EntryGenerator.h"
#include "ZipDirFs/Containers/EntryList.h"
#include "ZipDirFs/Fuse/NativeSymlink.h"
#include "ZipDirFs/Fuse/ZipRootDirectory.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Fuse
{
	NativeDirectory::NativeDirectory(const boost::filesystem::path& p) :
		path(p), _proxy(std::move(::ZipDirFs::Containers::EntryList<>::createWithProxy())),
		_changed(new ::ZipDirFs::Components::NativeChanged(path)),
		_generator(EntryGenerator::proxy_ptr(_proxy), EntryGenerator::changed_ptr(_changed),
			EntryGenerator::enumerator_ptr(
				new ::ZipDirFs::Components::NativeDirectoryEnumerator(path)),
			EntryGenerator::factory_ptr(new ::ZipDirFs::Components::NativeFactory<NativeDirectory,
				NativeSymlink, ZipRootDirectory>(path)))
	{
	}
	std::time_t NativeDirectory::getChangeTime() const { return *_changed; }
	std::time_t NativeDirectory::getModificationTime() const { return *_changed; }
	EntryGenerator& NativeDirectory::generator() { return _generator; }
	EntryGenerator::proxy_ptr& NativeDirectory::proxy() { return _proxy; }
} // namespace ZipDirFs::Fuse
