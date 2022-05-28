/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Zip/Entry.h"
#include "StateReporter.h"
#include "ZipDirFs/Zip/Factory.h"
#include "ZipDirFs/Zip/Lib.h"
#include <cstring>

#define ENTRY_IS_DIR 0
#define ENTRY_STATS_FETCHED 1

namespace ZipDirFs::Zip
{
	namespace
	{
		const std::streamsize chunksize = 4096;
		void ensureAvailable(Base::Content& content, std::streamsize position)
		{
			auto lock(content.readLock());
			if (content.lastWrite < position)
			{
				StateReporter::Log("entry").stream
					<< "Read zip file " << std::hex << (void*)content.data << " until " << std::dec
					<< position << " starting from current " << content.lastWrite << " ("
					<< Lib::ftell(content.data) << ")";
				while (content.lastWrite < position)
				{
					lock.makeWriter();
					auto len =
						Lib::fread(content.data, content.buffer + content.lastWrite, chunksize);
					content.lastWrite += len;
					if (content.lastWrite == content.length)
					{
						reportFileClosing(content.data);
						Lib::fclose(content.data);
						content.data = nullptr;
					}
					lock.makeReader();
				}
			}
		}
		std::int64_t directRead(Base::Content& content, void* buff, size_t size, off_t offset)
		{
			auto lock(content.writeLock());
			StateReporter::Log("entry").stream << "Read zip file " << std::hex
											   << (void*)content.data << " for " << std::dec << size
											   << " starting from " << offset;
			if (!Lib::fseek(content.data, offset, SEEK_SET)) {
				return -1;
			}
			int64_t total = 0, last = 0;
			while (size > 0 && (last = Lib::fread(content.data, buff, size)) > 0)
			{
				buff = (char*)buff + last;
				size -= last;
				total += last;
			}
			return total;
		}
	} // namespace
	Entry::Entry(const std::shared_ptr<Base::Lib>& d, const std::string& n, bool isDir) :
		data(d), name(n), cachedStat()
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
		{
			reportFileClosing(content.data);
			Lib::fclose(content.data);
		}
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
	void Entry::open()
	{
		auto lock(content.writeLock());
		if (content.buffer == nullptr && !content.directIO)
		{
			stat();
			content.data = Lib::fopen_index(data.get(), cachedStat.getIndex());
			content.directIO = !cachedStat.getCompressed();
			if (!content.directIO)
			{
				content.buffer = new char[content.length = cachedStat.getSize()];
			}
			reportFileOpened(data.get(), name, content.data);
		}
	}
	bool Entry::isDir() const { return flags[ENTRY_IS_DIR]; }
	std::int64_t Entry::read(void* buff, size_t size, off_t offset)
	{
		open();
		if (content.directIO)
		{
			return directRead(content, buff, size, offset);
		}
		auto readNeed = std::min((decltype(content.length))(offset + size), content.length);
		try
		{
			ensureAvailable(content, readNeed);
		}
		catch (std::ios::failure)
		{
			return -1;
		}
		if (offset < content.length)
		{
			std::memcpy(buff, content.buffer + offset, readNeed - offset);
		}
		return readNeed - offset;
	}
} // namespace ZipDirFs::Zip
