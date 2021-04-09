/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIPFILEBUFFER_H
#define ZIPDIRFS_ZIPFILEBUFFER_H

#include "ZipDirFs/ZipEntry.h"
#include <cstdint>
#include <asm-generic/errno-base.h>
#include <fuse/fuse.h>
#include <ctime>

namespace ZipDirFs
{
	template <class Derived>
	class ZipFileBuffer
	{
	public:
		ZipFileBuffer() : source(NULL) {}
		virtual ~ZipFileBuffer()
		{
			if (source != NULL)
			{
				delete source;
			}
		}
		int open(::fuse_file_info& fi)
		{
			if (!this->source->open()) { return -EIO; }
			return 0;
		}
		int close(::fuse_file_info& fi)
		{
			this->source->release();
			return 0;
		}
		int read(char* buf, ::size_t size, ::off_t offset, ::fuse_file_info& fi)
		{
			return this->source->read(buf, size, offset);
		}
		int write(const char* buf, ::size_t size, ::off_t offset, ::fuse_file_info& fi)
		{
			return -EACCES;
		}
		::uint64_t size()
		{
			return this->source->getSize();
		}
		int flush(::fuse_file_info& fi)
		{
			return 0;
		}
		int truncate(::off_t offset)
		{
			return -EACCES;
		}
		int readlink(char*, size_t)
		{
			return -EINVAL;
		}
		timespec getMTime()
		{
			return this->source->getMTime();
		}
		friend class ZipWalker;
	protected:
	private:
		ZipEntry* source;
		void setFileInfo(ZipEntry* source)
		{
			this->source = source;
		}

	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_ZIPFILEBUFFER_H
