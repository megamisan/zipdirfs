/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_FUSE_ZIPFILEBUFFER_H
#define ZIPDIRFS_FUSE_ZIPFILEBUFFER_H

#include "ZipDirFs/Zip/Entry.h"
#include "ZipDirFs/Zip/Exception.h"
#include "ZipDirFs/Zip/Stream.h"
#include <fuse.h>
#include <map>

namespace ZipDirFs::Fuse
{
	using ::ZipDirFs::Zip::Exception;
	using ::ZipDirFs::Zip::Stream;

	template <class Derived, class Entry = ::ZipDirFs::Zip::Entry>
	struct ZipFileBuffer
	{
		off_t size() { return entrySize(); }
		int open(fuse_file_info& fi)
		{
			if ((fi.flags & O_ACCMODE) != 0)
			{
				return -EACCES;
			}
			fi.fh = nextId++;
			try
			{
				auto pair = std::pair<int, std::unique_ptr<Stream>>(
					fi.fh, std::unique_ptr<Stream>(new Stream(*(entry()))));
				states.insert(std::move(pair));
			}
			catch (Exception e)
			{
				return -EIO;
			}
			return 0;
		}
		int close(fuse_file_info& fi) { return states.erase(fi.fh) == 1 ? 0 : -EBADF; }
		int read(char* buf, size_t size, off_t offset, fuse_file_info& fi)
		{
			auto it = states.find(fi.fh);
			if (it == states.end())
			{
				return -EBADF;
			}
			auto& stream = *(it->second);
			auto current = buf, end = buf + size;
			stream.seekg(offset);
			while (current < end && !stream.eof()) {
				stream.read(current, end - current);
				current += stream.gcount();
			}
			return current - buf;
		}
		int write(const char* buf, size_t size, off_t offset, fuse_file_info& fi) { return -EBADF; }
		int flush(fuse_file_info& fi) { return 0; }
		int truncate(off_t off) { return -EBADF; }
		int readlink(char* buf, size_t bufsize) { return -EINVAL; }

		virtual std::shared_ptr<Entry> entry() const = 0;
		virtual off_t entrySize() const = 0;

	protected:
		std::map<int, std::unique_ptr<Stream>> states;
		static uint64_t nextId;
	};

	template <class Derived, class Entry>
	uint64_t ZipFileBuffer<Derived, Entry>::nextId = 1;

	template <class Derived>
	struct DefaultZipFileBuffer : public ZipFileBuffer<Derived>
	{
	};
} // namespace ZipDirFs::Fuse

#endif // ZIPDIRFS_FUSE_ZIPFILEBUFFER_H
