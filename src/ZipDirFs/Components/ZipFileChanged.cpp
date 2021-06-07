/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Components/ZipFileChanged.h"
#include "ZipDirFs/Utilities/FileSystem.h"
#include "ZipDirFs/Zip/Factory.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Components
{
	using namespace ZipDirFs::Zip;
	using ZipDirFs::Utilities::FileSystem;

	namespace
	{
		const std::time_t getTimeReal(const boost::filesystem::path& p, const std::string& i,
			std::bitset<2>& state, std::time_t& pathChanged);
	}

	ZipFileChanged::ZipFileChanged(const boost::filesystem::path& path, const std::string& item) :
		path(path), item(item), lastPathChanged{0}, lastItemChanged{0}, state{0}
	{
	}

	bool ZipFileChanged::operator()()
	{
		std::time_t pathChanged, itemChanged = getTimeReal(path, item, state, pathChanged);
		bool result = pathChanged != lastPathChanged;
		lastPathChanged = pathChanged;
		if (state[1])
		{
			result = result || itemChanged != lastItemChanged;
			lastItemChanged = itemChanged;
		}
		return result;
	}

	ZipFileChanged::operator std::time_t() const
	{
		return state[1] ? lastItemChanged : lastPathChanged;
	}

	const std::time_t ZipFileChanged::getTime(
		const boost::filesystem::path& p, const std::string& i)
	{
		std::bitset<2> state{0};
		std::time_t pathChanged, itemChanged = getTimeReal(p, i, state, pathChanged);
		return state[1] ? itemChanged : pathChanged;
	}

	namespace
	{
		const std::time_t getTimeReal(const boost::filesystem::path& p, const std::string& i,
			std::bitset<2>& state, std::time_t& pathChanged)
		{
			std::shared_ptr<Archive> archive{nullptr};
			std::shared_ptr<Entry> entry{nullptr};
			pathChanged = FileSystem::last_write_time(p);
			if (!state[0])
			{
				auto archive = Factory::getInstance().get(p);
				state[0] = true;
				state[1] = (entry = archive->open(i)) != nullptr;
			}
			return state[1] ? Factory::getInstance().get(p)->open(i)->stat().getChanged() : 0;
		}
	} // namespace
} // namespace ZipDirFs::Components
