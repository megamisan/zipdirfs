/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Fuse/ZipRootDirectory.h"
#include "ZipDirFs/Components/NativeChanged.h"
#include "ZipDirFs/Components/ZipDirectoryEnumerator.h"
#include "ZipDirFs/Components/ZipFactory.h"
#include "ZipDirFs/Containers/EntryGenerator.h"
#include "ZipDirFs/Containers/EntryList.h"
#include "ZipDirFs/Fuse/ZipDirectory.h"
#include "ZipDirFs/Fuse/ZipFile.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Fuse
{
	std::string ZipRootDirectoryItem("");

	ZipRootDirectory::ZipRootDirectory(const boost::filesystem::path& p) :
		path(p), _proxy(std::move(::ZipDirFs::Containers::EntryList<>::createWithProxy())),
		_changed(new ::ZipDirFs::Components::NativeChanged(path)),
		_generator(EntryGenerator::proxy_ptr(_proxy), EntryGenerator::changed_ptr(_changed),
			EntryGenerator::enumerator_ptr(
				new ::ZipDirFs::Components::ZipDirectoryEnumerator(path, ZipRootDirectoryItem)),
			EntryGenerator::factory_ptr(
				new ::ZipDirFs::Components::ZipFactory<ZipDirectory, ZipFile>(path, ZipRootDirectoryItem, _changed)))
	{
	}
	std::time_t ZipRootDirectory::getChangeTime() const { return *_changed; }
	std::time_t ZipRootDirectory::getModificationTime() const { return *_changed; }
	EntryGenerator& ZipRootDirectory::generator() { return _generator; }
	EntryGenerator::proxy_ptr& ZipRootDirectory::proxy() { return _proxy; }
} // namespace ZipDirFs::Fuse
