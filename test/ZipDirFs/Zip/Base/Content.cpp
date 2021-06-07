/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "Content.h"
#include "test/gtest.h"
#include <thread>

namespace Test::ZipDirFs::Zip::Base
{
	using ::ZipDirFs::Zip::Base::Content;

	std::mutex& ContentAccess::getRead(Content& c)
	{
		return reinterpret_cast<ContentAccess*>(&c)->read;
	}
	std::mutex& ContentAccess::getWrite(Content& c)
	{
		return reinterpret_cast<ContentAccess*>(&c)->write;
	}
	std::condition_variable_any& ContentAccess::getZeroReaders(Content& c)
	{
		return reinterpret_cast<ContentAccess*>(&c)->zeroReaders;
	}
	std::uint64_t& ContentAccess::getReaderCount(Content& c)
	{
		return reinterpret_cast<ContentAccess*>(&c)->readerCount;
	}

	const std::chrono::seconds wait_time(1);
	const std::chrono::milliseconds sleep_time(250);

	TEST(Content_read_lock_Test, Type)
	{
		typedef std::is_base_of<std::unique_lock<std::mutex>, Content::read_lock> baseof;
		ASSERT_TRUE(baseof::value);
	}

	TEST(Content_read_lock_Test, Instantiate)
	{
		std::mutex tmp;
		Content::read_lock lock(tmp);
		ASSERT_EQ(lock.mutex(), &tmp);
	}

	TEST(Content_write_lock_Test, Type)
	{
		typedef std::is_base_of<std::unique_lock<std::mutex>, Content::write_lock> baseof;
		ASSERT_TRUE(baseof::value);
	}

	TEST(Content_write_lock_Test, Instantiate)
	{
		std::mutex tmp;
		Content::write_lock lock(tmp);
		ASSERT_EQ(lock.mutex(), &tmp);
	}

	TEST(ContentTest, Instantiate)
	{
		Content content;
		ASSERT_EQ(content.buffer, nullptr);
		ASSERT_EQ(content.data, nullptr);
		ASSERT_EQ(content.lastWrite, 0);
		ASSERT_EQ(content.length, 0);
		ASSERT_EQ(ContentAccess::getReaderCount(content), 0);
	}

	TEST(ContentTest, ReadLock)
	{
		Content content;
		auto lock(content.readLock());
		ASSERT_EQ(lock.mutex(), &ContentAccess::getRead(content));
	}

	TEST(ContentTest, WriteLock)
	{
		Content content;
		auto lock(content.writeLock());
		ASSERT_EQ(lock.mutex(), &ContentAccess::getWrite(content));
	}

	TEST(ContentTest, IncReader)
	{
		const std::uint64_t val = ::Test::rand(UINT32_MAX);
		const std::uint64_t next = val + 1;
		Content content;
		auto lock(content.readLock());
		ContentAccess::getReaderCount(content) = val;
		content.incReadersAtomic(lock);
		ASSERT_EQ(ContentAccess::getReaderCount(content), next);
	}

	TEST(ContentTest, DecReader)
	{
		const std::uint64_t val = []()
		{
			auto val = ::Test::rand(UINT32_MAX);
			while (val < 2)
				val = ::Test::rand(UINT32_MAX);
			return val;
		}();
		const std::uint64_t next = val - 1;
		Content content;
		auto lock(content.readLock());
		ContentAccess::getReaderCount(content) = val;
		content.decReadersAtomic(lock);
		ASSERT_EQ(ContentAccess::getReaderCount(content), next);
	}

	TEST(ContentTest, DecReaderLast)
	{
		bool passed = false;
		Content content;
		std::thread check(
			[&passed, &content]()
			{
				auto lock(content.readLock());
				if (ContentAccess::getZeroReaders(content).wait_for(lock, wait_time)
					== std::cv_status::no_timeout)
				{
					passed = true;
				}
			});
		std::this_thread::sleep_for(sleep_time);
		auto lock(content.readLock());
		ContentAccess::getReaderCount(content) = 1;
		content.decReadersAtomic(lock);
		lock.unlock();
		check.join();
		ASSERT_TRUE(passed);
	}

	TEST(ContentTest, WaitNoReadersZero)
	{
		bool timedout = false;
		Content content;
		std::thread check(
			[&timedout, &content]()
			{
				auto lock(content.readLock());
				if (ContentAccess::getZeroReaders(content).wait_for(lock, wait_time)
					== std::cv_status::timeout)
				{
					timedout = true;
				}
			});
		std::this_thread::sleep_for(sleep_time);
		auto lock(content.readLock());
		ContentAccess::getReaderCount(content) = 0;
		content.waitNoReaders(lock);
		lock.unlock();
		check.join();
		ASSERT_TRUE(timedout);
	}

	TEST(ContentTest, WaitNoReadersNotify)
	{
		std::vector<std::uint8_t> values{1, 2};
		std::uint8_t waiter = 0, trigerer = 0;
		std::mutex valuesExtract;
		auto pop = [&values, &valuesExtract](std::uint8_t& target)
		{
			std::lock_guard<std::mutex> lock(valuesExtract);
			target = values.back();
			values.pop_back();
		};
		Content content;
		std::thread check(
			[&pop, &waiter, &content]()
			{
				auto lock(content.readLock());
				ContentAccess::getReaderCount(content) = 1;
				content.waitNoReaders(lock);
				pop(waiter);
			});
		std::this_thread::sleep_for(sleep_time);
		auto lock(content.readLock());
		content.decReadersAtomic(lock);
		lock.unlock();
		std::this_thread::sleep_for(sleep_time);
		pop(trigerer);
		ContentAccess::getZeroReaders(content).notify_all();
		check.join();
		ASSERT_LT(trigerer, waiter);
	}
} // namespace Test::ZipDirFs::Zip::Base
