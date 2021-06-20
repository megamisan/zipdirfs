/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIP_BASE_CONTENT_H
#define ZIPDIRFS_ZIP_BASE_CONTENT_H

#include "ZipDirFs/Zip/Base/Lib.h"
#include <condition_variable>
#include <mutex>
#include <string>

namespace ZipDirFs::Zip::Base
{
	/**
	 * @brief Helper class to allow random access to zip archive file content
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	struct Content
	{
		typedef std::unique_lock<std::mutex> lock_type;
		struct lock
		{
			lock(lock&&);
			explicit lock(Content*, bool);
			~lock();
			void makeWriter();
			void makeReader();

		protected:
			Content* content;
			bool writer;
		};
		Content() noexcept;
		char* buffer;
		std::streamsize lastWrite;
		std::streamsize length;
		Lib::File* data;
		lock readLock();
		lock writeLock();

	protected:
		std::mutex global;
		std::condition_variable_any released;
		std::int32_t readersActive;
		std::uint32_t writersWaiting;
		friend class lock;
	};
} // namespace ZipDirFs::Zip::Base

#endif // ZIPDIRFS_ZIP_BASE_CONTENT_H
