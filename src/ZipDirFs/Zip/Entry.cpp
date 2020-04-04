/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Zip/Entry.h"
#include "ZipDirFs/Zip/Factory.h"
#include "ZipDirFs/Zip/Lib.h"

#define ENTRY_IS_DIR 0
#define ENTRY_STATS_FETCHED 1

namespace ZipDirFs::Zip
{
	Entry::Entry(const std::shared_ptr<Base::Lib>& d, const std::string& n, bool isDir) : data(d), name(n), cachedStat()
	{
		if (flags[ENTRY_IS_DIR] = flags[ENTRY_STATS_FETCHED] = isDir)
		{
			cachedStat = Base::Stat(-1, name, -1, 0, false);
		}
	}
	Entry::~Entry()
	{
		if (content.buffer != nullptr)
			delete[] content.buffer;
		if (content.data != nullptr)
			Lib::fclose(content.data);
	}
	const Base::Stat& Entry::stat()
	{
		if (!flags[ENTRY_STATS_FETCHED])
		{
			cachedStat = Lib::stat(data.get(), name);
			flags[ENTRY_STATS_FETCHED] = true;
		}
		return cachedStat;
	}
	std::shared_ptr<Base::Content> Entry::open()
	{
		auto lock(content.writeLock());
		if (content.buffer == nullptr)
		{
			stat();
			content.data = Lib::fopen_index(data.get(), cachedStat.getIndex());
			content.buffer = new char[content.length = cachedStat.getSize()];
		}
		return std::shared_ptr<Base::Content>(
			Factory::getInstance().get(data.get())->open(name), &content);
	}
	bool Entry::isDir() const { return flags[ENTRY_IS_DIR]; }
} // namespace ZipDirFs::Zip
