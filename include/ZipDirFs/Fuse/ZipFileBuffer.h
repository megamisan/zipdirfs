/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_FUSE_ZIPFILEBUFFER_H
#define ZIPDIRFS_FUSE_ZIPFILEBUFFER_H

#include "StateReporter.h"
#include "ZipDirFs/Zip/Entry.h"
#include "ZipDirFs/Zip/Exception.h"
#include <fuse.h>
#include <set>

namespace ZipDirFs::Fuse
{
	using ::ZipDirFs::Zip::Exception;
	typedef std::unique_lock<std::mutex> lock_type;

	/**
	 * @brief A fusekit buffer for zip archive file
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 * @tparam Derived Recursive templating parameter.
	 * @tparam Entry Implementation class of an entry of a zip archive.
	 */
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
			{
				StateReporter::Lock rl("NEXTID");
				rl.init();
				lock_type lock(idAccess);
				rl.set();
				fi.fh = nextId++;
			}
			try
			{
				StateReporter::Lock rl(buildId("FB", (std::uint64_t)(void*)this, 6));
				rl.init();
				lock_type lock(entryAccess);
				rl.set();
				handles.insert(fi.fh);
				entryRef = entry();
			}
			catch (Exception e)
			{
				return -EIO;
			}
			return 0;
		}
		int close(fuse_file_info& fi)
		{
			StateReporter::Lock rl(buildId("FB", (std::uint64_t)(void*)this, 6));
			rl.init();
			lock_type lock(entryAccess);
			rl.set();
			if (handles.find(fi.fh) != handles.end())
			{
				handles.erase(fi.fh);
				if (!handles.size())
				{
					entryRef = nullptr;
				}
				return 0;
			}
			return -EBADF;
		}
		int read(char* buf, size_t size, off_t offset, fuse_file_info& fi)
		{
			if (handles.find(fi.fh) == handles.end())
			{
				return -EBADF;
			}
			std::shared_ptr<Entry> e;
			{
				StateReporter::Lock rl(buildId("FB", (std::uint64_t)(void*)this, 6));
				rl.init();
				lock_type lock(entryAccess);
				rl.set();
				e = entryRef;
			}
			StateReporter::Log("filebuffer").stream << "Read file from " << offset << " for "
													<< size;
			if (e == nullptr)
			{
				return -EBADF;
			}
			auto read = e->read(buf, size, offset);
			return read >= 0 ? read : -EIO;
		}
		int write(const char* buf, size_t size, off_t offset, fuse_file_info& fi) { return -EBADF; }
		int flush(fuse_file_info& fi) { return 0; }
		int truncate(off_t off) { return -EBADF; }
		int readlink(char* buf, size_t bufsize) { return -EINVAL; }

		virtual std::shared_ptr<Entry> entry() const = 0;
		virtual off_t entrySize() const = 0;

	protected:
		std::set<int> handles;
		std::shared_ptr<Entry> entryRef;
		std::mutex entryAccess;
		static uint64_t nextId;
		static std::mutex idAccess;
	};

	template <class Derived, class Entry>
	uint64_t ZipFileBuffer<Derived, Entry>::nextId = 1;
	template <class Derived, class Entry>
	std::mutex ZipFileBuffer<Derived, Entry>::idAccess;

	/**
	 * @brief A fusekit buffer for zip archive file
	 * @remarks Uses the default @link ZipDirFs::Zip::Entry Entry @endlink.
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 * @tparam Derived Recursive templating parameter.
	 */
	template <class Derived>
	struct DefaultZipFileBuffer : public ZipFileBuffer<Derived>
	{
	};
} // namespace ZipDirFs::Fuse

#endif // ZIPDIRFS_FUSE_ZIPFILEBUFFER_H
