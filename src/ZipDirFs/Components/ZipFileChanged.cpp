/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Components/ZipFileChanged.h"
#include "ZipDirFs/Utilities/FileSystem.h"
#include "ZipDirFs/Zip/Factory.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Components
{
	using namespace ZipDirFs::Zip;
	using ZipDirFs::Utilities::FileSystem;

	ZipFileChanged::ZipFileChanged(const boost::filesystem::path& path, const std::string& item) :
		path(path), item(item), lastPathChanged{0}, lastItemChanged{0}, state{0}
	{
	}

	bool ZipFileChanged::operator()()
	{
		std::shared_ptr<Archive> archive{nullptr};
		auto lastWrite = FileSystem::last_write_time(path);
		bool result = lastWrite != lastPathChanged;
		lastPathChanged = lastWrite;
		if (!state[0])
		{
			auto archive = Factory::getInstance().get(path);
			state[0] = true;
			state[1] = archive->open(item) != nullptr;
		}
		if (state[1])
		{
			lastWrite = Factory::getInstance().get(path)->open(item)->stat().getChanged();
			result = result || lastWrite != lastItemChanged;
			lastItemChanged = lastWrite;
		}
		return result;
	}

	ZipFileChanged::operator std::time_t() const
	{
		return state[1] ? lastItemChanged : lastPathChanged;
	}
} // namespace ZipDirFs::Components
