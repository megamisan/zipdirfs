/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Zip/Base/Content.h"

namespace ZipDirFs::Zip::Base
{
	Content::Content() noexcept : buffer(nullptr), lastWrite(0), length(0), data(nullptr), readerCount(0) {}
	Content::read_lock Content::readLock() { return read_lock(read); }
	Content::write_lock Content::writeLock() { return write_lock(write); }
	void Content::incReadersAtomic(Content::read_lock& lock) noexcept
	{
		++readerCount;
	}
	void Content::decReadersAtomic(Content::read_lock& lock) noexcept
	{
		--readerCount;
		if (readerCount == 0)
		{
			zeroReaders.notify_all();
		}
	}
	void Content::waitNoReaders(Content::read_lock& lock)
	{
		if (readerCount != 0)
		{
			zeroReaders.wait(lock);
		}
	}
	Content::read_lock::read_lock(Content::read_lock::mutex_type& m): Content::lock_type(m) {}
	Content::write_lock::write_lock(Content::write_lock::mutex_type& m): Content::lock_type(m) {}
} // namespace ZipDirFs::Zip::Base
