/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "Content.h"
#include "test/gtest.h"
#include <thread>

namespace Test::ZipDirFs::Zip::Base
{
	using ::ZipDirFs::Zip::Base::Content;

	std::mutex& ContentAccess::getGlobal(Content& c)
	{
		return reinterpret_cast<ContentAccess*>(&c)->global;
	}
	std::condition_variable_any& ContentAccess::getReleased(Content& c)
	{
		return reinterpret_cast<ContentAccess*>(&c)->released;
	}
	std::int32_t& ContentAccess::getReadersActive(Content& c)
	{
		return reinterpret_cast<ContentAccess*>(&c)->readersActive;
	}
	std::uint32_t& ContentAccess::getWritersWaiting(Content& c)
	{
		return reinterpret_cast<ContentAccess*>(&c)->writersWaiting;
	}
	Content*& ContentLockAccess::getContent(Content::lock& l)
	{
		return reinterpret_cast<ContentLockAccess*>(&l)->content;
	}
	bool& ContentLockAccess::getWriter(Content::lock& l)
	{
		return reinterpret_cast<ContentLockAccess*>(&l)->writer;
	}

	const std::chrono::seconds wait_time(1);
	const std::chrono::milliseconds sleep_time(250);

	TEST(Content_lock_Test, InstantiateRead)
	{
		Content c;
		Content::lock lock(&c, false);
		EXPECT_EQ(ContentLockAccess::getContent(lock), &c);
		EXPECT_FALSE(ContentLockAccess::getWriter(lock));
		EXPECT_EQ(ContentAccess::getReadersActive(c), 1);
		EXPECT_EQ(ContentAccess::getWritersWaiting(c), 0);
		ContentLockAccess::getContent(lock) = nullptr;
	}

	TEST(Content_lock_Test, InstantiateReadNull)
	{
		Content::lock lock(nullptr, false);
		EXPECT_EQ(ContentLockAccess::getContent(lock), nullptr);
		EXPECT_FALSE(ContentLockAccess::getWriter(lock));
	}

	TEST(Content_lock_Test, InstantiateWrite)
	{
		Content c;
		Content::lock lock(&c, true);
		EXPECT_EQ(ContentLockAccess::getContent(lock), &c);
		EXPECT_TRUE(ContentLockAccess::getWriter(lock));
		EXPECT_EQ(ContentAccess::getReadersActive(c), -1);
		EXPECT_EQ(ContentAccess::getWritersWaiting(c), 0);
		ContentLockAccess::getContent(lock) = nullptr;
	}

	TEST(Content_lock_Test, InstantiateWriteNull)
	{
		Content::lock lock(nullptr, true);
		EXPECT_EQ(ContentLockAccess::getContent(lock), nullptr);
		EXPECT_TRUE(ContentLockAccess::getWriter(lock));
	}

	TEST(Content_lock_Test, InstantiateMove)
	{
		Content* pc = reinterpret_cast<Content*>(::Test::rand(UINT32_MAX));
		Content::lock firstLock(nullptr, true);
		ContentLockAccess::getContent(firstLock) = pc;
		Content::lock secondLock(std::move(firstLock));
		EXPECT_EQ(ContentLockAccess::getContent(firstLock), nullptr);
		EXPECT_EQ(ContentLockAccess::getContent(secondLock), pc);
		EXPECT_TRUE(ContentLockAccess::getWriter(secondLock));
		ContentLockAccess::getContent(firstLock) = nullptr;
		ContentLockAccess::getContent(secondLock) = nullptr;
	}

	TEST(Content_lock_Test, ConvertRead)
	{
		Content c;
		Content::lock lock(nullptr, false);
		ContentLockAccess::getContent(lock) = &c;
		ContentLockAccess::getWriter(lock) = true;
		ContentAccess::getReadersActive(c) = -1;
		ContentAccess::getWritersWaiting(c) = 0;
		std::mutex m;
		std::condition_variable_any start;
		bool timedout = false;
		std::unique_lock<std::mutex> l(m);
		std::thread t(
			[](Content& c, std::condition_variable_any& st, std::mutex& m, bool& timedout)
			{
				std::unique_lock<std::mutex> l(m), lock(ContentAccess::getGlobal(c));
				st.notify_all();
				l.unlock();
				if (ContentAccess::getReleased(c).wait_for(lock, std::chrono::milliseconds(250))
					== std::cv_status::timeout)
				{
					timedout = true;
				}
			},
			std::ref(c), std::ref(start), std::ref(m), std::ref(timedout));
		start.wait(l);
		l.unlock();
		lock.makeReader();
		t.join();
		EXPECT_FALSE(timedout);
		EXPECT_EQ(ContentLockAccess::getContent(lock), &c);
		EXPECT_FALSE(ContentLockAccess::getWriter(lock));
		EXPECT_EQ(ContentAccess::getReadersActive(c), 1);
		EXPECT_EQ(ContentAccess::getWritersWaiting(c), 0);
		ContentLockAccess::getContent(lock) = nullptr;
	}

	TEST(Content_lock_Test, ConvertReadOneReader)
	{
		Content c;
		Content::lock lock(nullptr, false);
		ContentLockAccess::getContent(lock) = &c;
		ContentLockAccess::getWriter(lock) = true;
		ContentAccess::getReadersActive(c) = -1;
		ContentAccess::getWritersWaiting(c) = 0;
		std::mutex m;
		std::condition_variable_any recover, start;
		bool timedout = false;
		std::unique_lock<std::mutex> l(m);
		std::thread t(
			[](Content& c, std::condition_variable_any& cv, std::condition_variable_any& st,
				std::mutex& m, bool& timedout)
			{
				std::unique_lock<std::mutex> lock(m);
				st.notify_all();
				if (cv.wait_for(lock, std::chrono::milliseconds(250)) == std::cv_status::timeout)
				{
					ContentAccess::getReleased(c).notify_all();
					timedout = true;
				}
			},
			std::ref(c), std::ref(recover), std::ref(start), std::ref(m), std::ref(timedout));
		start.wait(l);
		l.unlock();
		lock.makeReader();
		recover.notify_all();
		t.join();
		EXPECT_FALSE(timedout);
		EXPECT_EQ(ContentLockAccess::getContent(lock), &c);
		EXPECT_FALSE(ContentLockAccess::getWriter(lock));
		EXPECT_EQ(ContentAccess::getReadersActive(c), 1);
		EXPECT_EQ(ContentAccess::getWritersWaiting(c), 0);
		ContentLockAccess::getContent(lock) = nullptr;
	}

	TEST(Content_lock_Test, ConvertReadOneWriter)
	{
		Content c;
		Content::lock lock(nullptr, false);
		ContentLockAccess::getContent(lock) = &c;
		ContentLockAccess::getWriter(lock) = true;
		ContentAccess::getReadersActive(c) = -1;
		ContentAccess::getWritersWaiting(c) = 1;
		std::mutex m;
		std::condition_variable_any recover, start;
		bool timedout = false;
		std::unique_lock<std::mutex> l(m);
		std::thread t(
			[](Content& c, std::condition_variable_any& cv, std::condition_variable_any& st,
				std::mutex& m, bool& timedout)
			{
				std::unique_lock<std::mutex> lock(m);
				st.notify_all();
				if (cv.wait_for(lock, std::chrono::milliseconds(250)) == std::cv_status::timeout)
				{
					ContentAccess::getWritersWaiting(c) = 0;
					ContentAccess::getReleased(c).notify_all();
					timedout = true;
				}
			},
			std::ref(c), std::ref(recover), std::ref(start), std::ref(m), std::ref(timedout));
		start.wait(l);
		l.unlock();
		lock.makeReader();
		recover.notify_all();
		t.join();
		EXPECT_TRUE(timedout);
		EXPECT_EQ(ContentLockAccess::getContent(lock), &c);
		EXPECT_FALSE(ContentLockAccess::getWriter(lock));
		EXPECT_EQ(ContentAccess::getReadersActive(c), 1);
		EXPECT_EQ(ContentAccess::getWritersWaiting(c), 0);
		ContentLockAccess::getContent(lock) = nullptr;
	}

	TEST(Content_lock_Test, ConvertWrite)
	{
		Content c;
		Content::lock lock(nullptr, false);
		ContentLockAccess::getContent(lock) = &c;
		ContentLockAccess::getWriter(lock) = false;
		ContentAccess::getReadersActive(c) = 1;
		ContentAccess::getWritersWaiting(c) = 0;
		lock.makeWriter();
		EXPECT_EQ(ContentLockAccess::getContent(lock), &c);
		EXPECT_TRUE(ContentLockAccess::getWriter(lock));
		EXPECT_EQ(ContentAccess::getReadersActive(c), -1);
		EXPECT_EQ(ContentAccess::getWritersWaiting(c), 0);
		ContentLockAccess::getContent(lock) = nullptr;
	}

	TEST(Content_lock_Test, ConvertWriteOneReader)
	{
		Content c;
		Content::lock lock(nullptr, false);
		ContentLockAccess::getContent(lock) = &c;
		ContentLockAccess::getWriter(lock) = false;
		ContentAccess::getReadersActive(c) = 2;
		ContentAccess::getWritersWaiting(c) = 0;
		std::mutex m;
		std::condition_variable_any start;
		bool timedout = false;
		std::unique_lock<std::mutex> l(m);
		std::int32_t intReadersActive = 2;
		std::uint32_t intWritersWaiting = 0;
		std::thread t(
			[&intReadersActive, &intWritersWaiting](
				Content& c, std::condition_variable_any& st, std::mutex& m, bool& timedout)
			{
				std::unique_lock<std::mutex> l(m), lock(ContentAccess::getGlobal(c));
				st.notify_all();
				l.unlock();
				if (ContentAccess::getReleased(c).wait_for(lock, std::chrono::milliseconds(250))
					== std::cv_status::timeout)
				{
					timedout = true;
					intReadersActive = ContentAccess::getReadersActive(c);
					intWritersWaiting = ContentAccess::getWritersWaiting(c);
				}
				ContentAccess::getReadersActive(c) = 0;
				ContentAccess::getReleased(c).notify_all();
			},
			std::ref(c), std::ref(start), std::ref(m), std::ref(timedout));
		start.wait(l);
		l.unlock();
		lock.makeWriter();
		t.join();
		EXPECT_TRUE(timedout);
		EXPECT_EQ(ContentLockAccess::getContent(lock), &c);
		EXPECT_TRUE(ContentLockAccess::getWriter(lock));
		EXPECT_EQ(intReadersActive, 1);
		EXPECT_EQ(intWritersWaiting, 1);
		EXPECT_EQ(ContentAccess::getReadersActive(c), -1);
		EXPECT_EQ(ContentAccess::getWritersWaiting(c), 0);
		ContentLockAccess::getContent(lock) = nullptr;
	}

	TEST(Content_lock_Test, ConvertWriteOneWriter)
	{
		Content c;
		Content::lock lock(nullptr, false);
		ContentLockAccess::getContent(lock) = &c;
		ContentLockAccess::getWriter(lock) = false;
		ContentAccess::getReadersActive(c) = 1;
		ContentAccess::getWritersWaiting(c) = 1;
		std::mutex m;
		std::condition_variable_any recover, start;
		bool timedout = false;
		std::unique_lock<std::mutex> l(m);
		std::thread t(
			[](Content& c, std::condition_variable_any& cv, std::condition_variable_any& st,
				std::mutex& m, bool& timedout)
			{
				std::unique_lock<std::mutex> lock(m);
				st.notify_all();
				if (cv.wait_for(lock, std::chrono::milliseconds(250)) == std::cv_status::timeout)
				{
					timedout = true;
				}
			},
			std::ref(c), std::ref(recover), std::ref(start), std::ref(m), std::ref(timedout));
		start.wait(l);
		l.unlock();
		lock.makeWriter();
		recover.notify_all();
		t.join();
		EXPECT_FALSE(timedout);
		EXPECT_EQ(ContentLockAccess::getContent(lock), &c);
		EXPECT_TRUE(ContentLockAccess::getWriter(lock));
		EXPECT_EQ(ContentAccess::getReadersActive(c), -1);
		EXPECT_EQ(ContentAccess::getWritersWaiting(c), 1);
		ContentLockAccess::getContent(lock) = nullptr;
	}

	TEST(Content_lock_Test, FreeRead)
	{
		Content c;
		{
			Content::lock lock(nullptr, false);
			ContentLockAccess::getContent(lock) = &c;
			ContentAccess::getReadersActive(c) = 1;
		}
		EXPECT_EQ(ContentAccess::getReadersActive(c), 0);
		EXPECT_EQ(ContentAccess::getWritersWaiting(c), 0);
	}

	TEST(Content_lock_Test, FreeWrite)
	{
		Content c;
		{
			Content::lock lock(nullptr, true);
			ContentLockAccess::getContent(lock) = &c;
			ContentAccess::getReadersActive(c) = -1;
		}
		EXPECT_EQ(ContentAccess::getReadersActive(c), 0);
		EXPECT_EQ(ContentAccess::getWritersWaiting(c), 0);
	}

	TEST(ContentTest, Instantiate)
	{
		Content content;
		ASSERT_EQ(content.buffer, nullptr);
		ASSERT_EQ(content.data, nullptr);
		ASSERT_EQ(content.lastWrite, 0);
		ASSERT_EQ(content.length, 0);
		ASSERT_EQ(content.directIO, false);
		ASSERT_EQ(ContentAccess::getReadersActive(content), 0);
		ASSERT_EQ(ContentAccess::getWritersWaiting(content), 0);
	}

	TEST(ContentTest, ReadLock)
	{
		Content content;
		auto lock(content.readLock());
		EXPECT_EQ(ContentLockAccess::getContent(lock), &content);
		EXPECT_FALSE(ContentLockAccess::getWriter(lock));
		EXPECT_EQ(ContentAccess::getReadersActive(content), 1);
		EXPECT_EQ(ContentAccess::getWritersWaiting(content), 0);
	}

	TEST(ContentTest, WriteLock)
	{
		Content content;
		auto lock(content.writeLock());
		EXPECT_EQ(ContentLockAccess::getContent(lock), &content);
		EXPECT_TRUE(ContentLockAccess::getWriter(lock));
		EXPECT_EQ(ContentAccess::getReadersActive(content), -1);
		EXPECT_EQ(ContentAccess::getWritersWaiting(content), 0);
	}
} // namespace Test::ZipDirFs::Zip::Base
