/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIP_BASE_CONTENT_H
#define ZIPDIRFS_ZIP_BASE_CONTENT_H

#include "ZipDirFs/Zip/Base/Lib.h"
#include <condition_variable>
#include <mutex>
#include <string>

namespace ZipDirFs::Zip::Base
{
	struct Content
	{
		typedef std::unique_lock<std::mutex> lock_type;
		struct read_lock : public lock_type
		{
			read_lock(read_lock&&) = default;
			explicit read_lock(mutex_type&);
		};
		struct write_lock : public lock_type
		{
			write_lock(write_lock&&) = default;
			explicit write_lock(mutex_type&);
		};
		Content() noexcept;
		char* buffer;
		std::streamsize lastWrite;
		std::streamsize length;
		Lib::File* data;
		read_lock readLock();
		write_lock writeLock();
		void incReadersAtomic(read_lock&) noexcept;
		void decReadersAtomic(read_lock&) noexcept;
		void waitNoReaders(read_lock&);

	protected:
		std::mutex read;
		std::mutex write;
		std::condition_variable_any zeroReaders;
		std::uint64_t readerCount;
	};
} // namespace ZipDirFs::Zip::Base

#endif // ZIPDIRFS_ZIP_BASE_CONTENT_H
