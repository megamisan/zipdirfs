/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Fuse/ZipFile.h"
#include "ZipDirFs/Zip/Factory.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Fuse
{
	ZipFile::ZipFile(const boost::filesystem::path& p, const std::string& f,
		std::shared_ptr<::ZipDirFs::Containers::Helpers::Changed>& rc) :
		zip(p),
		file(f), _rootChanged(rc), changed(zip, file), size(entry()->stat().getSize())
	{
		changed();
	}
	std::time_t ZipFile::getParentModificationTime() const { return *_rootChanged; }
	std::time_t ZipFile::getModificationTime() const { return changed; }
	std::shared_ptr<::ZipDirFs::Zip::Entry> ZipFile::entry() const
	{
		return ::ZipDirFs::Zip::Factory::getInstance().get(zip)->open(file);
	}
	off_t ZipFile::entrySize() const { return size; }
} // namespace ZipDirFs::Fuse
