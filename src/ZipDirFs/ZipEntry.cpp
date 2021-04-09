/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/ZipEntry.h"
#include "ZipDirFs/ZipFile.h"
#include "ZipDirFs/ZipEntryFileInfo.h"
#include <zip.h>
#include <asm-generic/errno-base.h>
#include <cstring>

namespace ZipDirFs
{
	ZipEntry::ZipEntry(ZipFile& file, const ZipEntryFileInfo& fileinfo) : file(file), index(fileinfo.index), size(fileinfo.size), mtime( {fileinfo.mtime, 0}), buffer(NULL), progress(0), refCount(0), zipFileEntry(0)
	{
	}

	ZipEntry::~ZipEntry()
	{
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	bool ZipEntry::release()
	{
		Lock lock(*this);
		this->refCount--;

		if (this->refCount == 0)
		{
			delete[] this->buffer;
			this->buffer = NULL;
			this->progress = 0;
			::zip_fclose(this->zipFileEntry);
			this->zipFileEntry = NULL;
			this->file.releaseZip();
		}

		return true;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	bool ZipEntry::open()
	{
		Lock lock(*this);

		if (this->refCount == 0)
		{
			this->buffer = new char[this->size];

			if (this->buffer == NULL) { return false; }

			this->zipFileEntry = ::zip_fopen_index(this->file.getZip(), this->index, 0);

			if (this->zipFileEntry == NULL)
			{
				this->file.releaseZip();
				delete [] this->buffer;
				return false;
			}

			this->refCount = 1;
		}

		return true;
	}

	int ZipEntry::read(char* buf, ::size_t size, ::off_t offset)
	{
		if (offset > (::off_t)this->size) { offset = (::off_t)this->size; }
		if (offset + (::off_t)size >= (::off_t) this->size) { size = (::off_t) this->size - offset; }

		if (!this->ensureRead(size + offset)) { return -EIO; }

		memcpy(buf, this->buffer + offset, size);
		return size;
	}

	bool ZipEntry::ensureRead(::uint64_t position)
	{
		Lock lock(*this);

		if (position < this->progress) { return true; }

		int read;

		while (this->progress < position)
		{
			read = ::zip_fread(this->zipFileEntry, this->buffer + this->progress, position - this->progress);

			if (read < 0) { return false; }

			this->progress += read;
		}

		return true;
	}
} // namespace ZipDirFs
