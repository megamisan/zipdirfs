/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Fuse/ZipDirectory.h"
#include "ZipDirFs/Components/ZipDirectoryEnumerator.h"
#include "ZipDirFs/Components/ZipFactory.h"
#include "ZipDirFs/Components/ZipFileChanged.h"
#include "ZipDirFs/Containers/EntryGenerator.h"
#include "ZipDirFs/Containers/EntryList.h"
#include "ZipDirFs/Fuse/ZipFile.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Fuse
{
	ZipDirectory::ZipDirectory(
		const boost::filesystem::path& p, const std::string& f, EntryGenerator::changed_ptr& rc) :
		zip(p),
		folder(f), _rootChanged(rc),
		_proxy(std::move(::ZipDirFs::Containers::EntryList<>::createWithProxy())),
		_changed(new ::ZipDirFs::Components::ZipFileChanged(zip, folder)),
		_generator(EntryGenerator::proxy_ptr(_proxy), EntryGenerator::changed_ptr(_changed),
			EntryGenerator::enumerator_ptr(
				new ::ZipDirFs::Components::ZipDirectoryEnumerator(zip, folder)),
			EntryGenerator::factory_ptr(
				new ::ZipDirFs::Components::ZipFactory<ZipDirectory, ZipFile>(zip, folder, rc)))
	{
	}
	std::time_t ZipDirectory::getParentModificationTime() const { return *_rootChanged; }
	std::time_t ZipDirectory::getModificationTime() const { return *_changed != 0 ? *_changed : *_rootChanged; }
	EntryGenerator& ZipDirectory::generator() { return _generator; }
	EntryGenerator::proxy_ptr& ZipDirectory::proxy() { return _proxy; }
} // namespace ZipDirFs::Fuse
