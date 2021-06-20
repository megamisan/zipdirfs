/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Zip/Base/Content.h"

namespace ZipDirFs::Zip::Base
{
	Content::Content() noexcept :
		buffer(nullptr), lastWrite(0), length(0), data(nullptr), readersActive(0), writersWaiting(0)
	{
	}
	Content::lock Content::readLock() { return lock(this, false); }
	Content::lock Content::writeLock() { return lock(this, true); }
	Content::lock::lock(Content* c, bool w) : content(c), writer(w)
	{
		if (content != nullptr)
		{
			std::unique_lock<std::mutex> guard(content->global);
			if (writer)
			{
				++content->writersWaiting;
				while (content->readersActive != 0)
				{
					content->released.wait(guard);
				}
				--content->writersWaiting;
				--content->readersActive;
			}
			else
			{
				while (content->writersWaiting > 0 || content->readersActive < 0)
				{
					content->released.wait(guard);
				}
				++content->readersActive;
			}
		}
	}
	Content::lock::lock(lock&& l)
	{
		content = l.content;
		writer = l.writer;
		l.content = nullptr;
	}
	Content::lock::~lock()
	{
		if (content != nullptr)
		{
			std::unique_lock<std::mutex> guard(content->global);
			if (writer)
			{
				++content->readersActive;
				content->released.notify_all();
			}
			else
			{
				--content->readersActive;
				if (content->readersActive == 0)
				{
					content->released.notify_all();
				}
			}
		}
	}
	void Content::lock::makeWriter()
	{
		if (content != nullptr)
		{
			std::unique_lock<std::mutex> guard(content->global);
			if (!writer)
			{
				--content->readersActive;
				++content->writersWaiting;
				writer = true;
				while (content->readersActive != 0)
				{
					content->released.wait(guard);
				}
				--content->writersWaiting;
				--content->readersActive;
			}
		}
	}
	void Content::lock::makeReader()
	{
		if (content != nullptr)
		{
			std::unique_lock<std::mutex> guard(content->global);
			if (writer)
			{
				++content->readersActive;
				writer = false;
				content->released.notify_all();
				while (content->writersWaiting > 0 || content->readersActive < 0)
				{
					content->released.wait(guard);
				}
				++content->readersActive;
			}
		}
	}
} // namespace ZipDirFs::Zip::Base
