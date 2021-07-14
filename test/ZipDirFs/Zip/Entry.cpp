/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "Entry.h"
#include "Fixtures/SimpleSpan.h"
#include "test/gtest.h"
#include <boost/filesystem.hpp>
#include <csignal>
#include <functional>
#include <gmock/gmock.h>

namespace ZipDirFs::Zip::Base
{
	bool operator==(const Content& c1, const Content& c2)
	{
		return c1.buffer == c2.buffer && c1.data == c2.data && c1.lastWrite == c2.lastWrite
			&& c1.length == c2.length;
	}

	bool operator==(const Stat& s1, const Stat& s2)
	{
		return s1.getChanged() == s2.getChanged() && s1.getCompressed() == s2.getCompressed()
			&& s1.getIndex() == s2.getIndex() && s1.getName() == s2.getName()
			&& s1.getSize() == s2.getSize();
	}
} // namespace ZipDirFs::Zip::Base

namespace Test::ZipDirFs::Zip
{
	using ::testing::_;
	using ::testing::Return;
	using ::testing::ReturnArg;
	using ::ZipDirFs::Zip::Entry;
	using ::ZipDirFs::Zip::Base::Content;
	template <Fixtures::SimpleSpanImpl::size_type size>
	using SimpleSpan = Fixtures::SimpleSpan<char, size>;

	namespace
	{
		constexpr std::uint64_t randomLenModulo() { return 1048576; }
		constexpr std::uint64_t randomLenBase() { return 4096; }
		constexpr std::uint64_t randomLenMax() { return randomLenModulo() + randomLenBase() - 1; }

		void GenerateRandomSegment(char* buffer, std::streamsize len)
		{
			using rand_type = double;
			const std::streamsize last = len - (len % sizeof(rand_type));
			rand_type* dest = reinterpret_cast<decltype(dest)>(buffer);
			rand_type* end = reinterpret_cast<decltype(end)>(buffer) + last / sizeof(rand_type);
			while (dest < end)
			{
				*(dest++) = ::Test::rand();
			}
			if (last != len)
			{
				rand_type value = ::Test::rand();
				std::memcpy(dest, &value, len - last);
			}
		}

		std::unique_ptr<char, std::function<void(char*)>> GenerateStartContentState(
			Content& content)
		{
			auto c = &content;
			content.length = ::Test::rand(randomLenBase(), randomLenMax() - 1);
			content.lastWrite = 0;
			std::unique_ptr<char, std::function<void(char*)>> buffer(new char[content.length],
				[c](char* buf)
				{
					delete[] buf;
					c->buffer = nullptr;
				});
			content.buffer = buffer.get();
			return buffer;
		}

		std::unique_ptr<char, std::function<void(char*)>> GenerateRandomBufferedContentState(
			Content& content)
		{
			auto c = &content;
			content.length = ::Test::rand(randomLenBase(), randomLenMax() - 1);
			content.lastWrite = ::Test::rand(long(randomLenBase()), content.length);
			std::unique_ptr<char, std::function<void(char*)>> buffer(new char[content.length],
				[c](char* buf)
				{
					delete[] buf;
					c->buffer = nullptr;
				});
			GenerateRandomSegment(content.buffer = buffer.get(), content.length);
			return buffer;
		}

	} // namespace

	const std::shared_ptr<LibBase>& EntryAccess::getData(Entry& e)
	{
		return reinterpret_cast<EntryAccess*>(&e)->data;
	}
	const std::string& EntryAccess::getName(Entry& e)
	{
		return reinterpret_cast<EntryAccess*>(&e)->name;
	}
	std::bitset<2>& EntryAccess::getFlags(Entry& e)
	{
		return reinterpret_cast<EntryAccess*>(&e)->flags;
	}
	Stat& EntryAccess::getCachedStat(Entry& e)
	{
		return reinterpret_cast<EntryAccess*>(&e)->cachedStat;
	}
	Content& EntryAccess::getContent(Entry& e)
	{
		return reinterpret_cast<EntryAccess*>(&e)->content;
	}
	void EntryAccess::invokeOpen(Entry& e) { reinterpret_cast<EntryAccess*>(&e)->open(); }

	void EntryTest::SetUp()
	{
		data = std::shared_ptr<LibBase>(
			reinterpret_cast<LibBase*>(::Test::rand(UINT32_MAX)), [](LibBase*) {});
	}

	TEST_F(EntryTest, InstantiateFile)
	{
		std::string name(std::string("file") + std::to_string(::Test::rand(UINT32_MAX)));
		Entry entry(data, name, false);
		EXPECT_EQ(EntryAccess::getData(entry), data);
		EXPECT_EQ(EntryAccess::getName(entry), name);
		EXPECT_EQ(EntryAccess::getFlags(entry), std::bitset<2>(0));
		EXPECT_EQ(EntryAccess::getCachedStat(entry), Stat());
		EXPECT_EQ(EntryAccess::getContent(entry), Content());
	}

	TEST_F(EntryTest, InstantiateFolder)
	{
		std::string name(std::string("folder") + std::to_string(::Test::rand(UINT32_MAX)));
		Entry entry(data, name, true);
		EXPECT_EQ(EntryAccess::getData(entry), data);
		EXPECT_EQ(EntryAccess::getName(entry), name);
		EXPECT_EQ(EntryAccess::getFlags(entry), std::bitset<2>(3));
		EXPECT_EQ(EntryAccess::getCachedStat(entry), Stat(-1, name, -1, 0, false));
		EXPECT_EQ(EntryAccess::getContent(entry), Content());
	}

	TEST_F(EntryTest, DestroyWithBuffer)
	{
		EXPECT_EXIT(
			{
				Entry entry(
					data, std::string("file") + std::to_string(::Test::rand(UINT32_MAX)), false);
				char c;
				EntryAccess::getContent(entry).buffer = &c;
			},
			[](int status)
			{
				return ::testing::KilledBySignal(SIGABRT)(status)
					|| ::testing::KilledBySignal(SIGSEGV)(status);
			},
			"");
	}

	TEST_F(EntryTest, DestroyWithFile)
	{
		Fixtures::Lib lib;
		Entry entry(data, std::string("file") + std::to_string(::Test::rand(UINT32_MAX)), false);
		::ZipDirFs::Zip::Base::Lib::File* file =
			reinterpret_cast<decltype(file)>(::Test::rand(UINT32_MAX));
		EXPECT_CALL(lib, fclose(file));
		EntryAccess::getContent(entry).data = file;
	}

	TEST_F(EntryTest, StatFolder)
	{
		std::string name(std::string("folder") + std::to_string(::Test::rand(UINT32_MAX)));
		Entry entry(data, name, true);
		Stat result = entry.stat();
		EXPECT_EQ(result, Stat(-1, name, -1, 0, false));
	}

	TEST_F(EntryTest, StatFileFetch)
	{
		Fixtures::Lib lib;
		std::string name(std::string("file") + std::to_string(::Test::rand(UINT32_MAX)));
		Stat expected(::Test::rand(UINT32_MAX), name, ::Test::rand(UINT32_MAX),
			::Test::rand(UINT32_MAX), ::Test::rand(1));
		Entry entry(data, name, false);
		EXPECT_CALL(lib, stat(data.get(), name)).WillOnce(Return(expected));
		Stat result = entry.stat();
		EXPECT_EQ(result, expected);
	}

	TEST_F(EntryTest, StatFileFetched)
	{
		std::string name(std::string("file") + std::to_string(::Test::rand(UINT32_MAX)));
		Stat expected(::Test::rand(UINT32_MAX), name, ::Test::rand(UINT32_MAX),
			::Test::rand(UINT32_MAX), ::Test::rand(1));
		Entry entry(data, name, true);
		EntryAccess::getCachedStat(entry) = expected;
		EntryAccess::getFlags(entry)[1] = true;
		Stat result = entry.stat();
		EXPECT_EQ(result, expected);
	}

	TEST_F(EntryTest, OpenNoBuffer)
	{
		Fixtures::Lib lib;
		std::string name(std::string("file") + std::to_string(::Test::rand(UINT32_MAX)));
		Stat stat(::Test::rand(UINT32_MAX), name, ::Test::rand(UINT32_MAX) % 20,
			::Test::rand(UINT32_MAX), ::Test::rand(1));
		::ZipDirFs::Zip::Base::Lib::File* file(
			reinterpret_cast<decltype(file)>(::Test::rand(UINT32_MAX)));
		Entry entry(data, name, false);
		EntryAccess::getCachedStat(entry) = stat;
		EntryAccess::getFlags(entry)[1] = true;
		EXPECT_CALL(lib, fopen_index(data.get(), stat.getIndex())).WillOnce(Return(file));
		std::shared_ptr<Entry> pentry(
			&entry, [](Entry* e) { EntryAccess::getContent(*e).data = nullptr; });
		EntryAccess::invokeOpen(entry);
		EXPECT_EQ(EntryAccess::getContent(entry).data, file);
		EXPECT_NE(EntryAccess::getContent(entry).buffer, nullptr);
		EXPECT_EQ(EntryAccess::getContent(entry).length, stat.getSize());
	}

	TEST_F(EntryTest, OpenBuffer)
	{
		Fixtures::Lib lib;
		std::string name(std::string("file") + std::to_string(::Test::rand(UINT32_MAX)));
		Entry entry(data, name, false);
		std::unique_ptr<char, std::function<void(char*)>> buffer(
			reinterpret_cast<char*>(::Test::rand(UINT32_MAX)),
			[&entry](char*) { EntryAccess::getContent(entry).buffer = nullptr; });
		EntryAccess::getContent(entry).buffer = buffer.get();
		EXPECT_EQ(buffer.get(), EntryAccess::getContent(entry).buffer);
	}

	TEST_F(EntryTest, IsDirFile)
	{
		std::string name(std::string("file") + std::to_string(::Test::rand(UINT32_MAX)));
		Entry entry(data, name, false);
		EXPECT_FALSE(entry.isDir());
	}

	TEST_F(EntryTest, IsDirFolder)
	{
		std::string name(std::string("folder") + std::to_string(::Test::rand(UINT32_MAX)));
		Entry entry(data, name, true);
		EXPECT_TRUE(entry.isDir());
	}

	TEST_F(EntryTest, ReadStart)
	{
		Fixtures::Lib lib;
		::ZipDirFs::Zip::Base::Lib::File* file(
			reinterpret_cast<decltype(file)>(::Test::rand(UINT32_MAX)));
		std::string name(std::string("file") + std::to_string(::Test::rand(UINT32_MAX)));
		Entry entry(data, name, false);
		EntryAccess::getContent(entry).data = file;
		std::shared_ptr<Entry> pentry(
			&entry, [](Entry* e) { EntryAccess::getContent(*e).data = nullptr; });
		auto buffer = GenerateStartContentState(EntryAccess::getContent(entry));
		EXPECT_CALL(lib, fread(file, buffer.get(), randomLenBase()))
			.WillOnce(Return(randomLenBase()));
		char tmp[randomLenBase()];
		EXPECT_EQ(randomLenBase(), entry.read(tmp, randomLenBase(), 0));
	}

	TEST_F(EntryTest, ReadAvailable)
	{
		using Fixtures::dynamic_extend;
		Fixtures::Lib lib;
		::ZipDirFs::Zip::Base::Lib::File* file(
			reinterpret_cast<decltype(file)>(::Test::rand(UINT32_MAX)));
		std::string name(std::string("file") + std::to_string(::Test::rand(UINT32_MAX)));
		Entry entry(data, name, false);
		EntryAccess::getContent(entry).data = file;
		std::shared_ptr<Entry> pentry(
			&entry, [](Entry* e) { EntryAccess::getContent(*e).data = nullptr; });
		auto buffer = GenerateRandomBufferedContentState(EntryAccess::getContent(entry));
		char tmp[randomLenBase()];
		EXPECT_EQ(randomLenBase(), entry.read(tmp, randomLenBase(), 0));
		EXPECT_EQ(SimpleSpan<dynamic_extend>(buffer.get(), randomLenBase()).content(),
			SimpleSpan<dynamic_extend>(tmp, randomLenBase()).content());
	}

	TEST_F(EntryTest, ReadUnavailableOneChunk)
	{
		using Fixtures::dynamic_extend;
		// TODO: One chunk / Two chunks
		Fixtures::Lib lib;
		::ZipDirFs::Zip::Base::Lib::File* file(
			reinterpret_cast<decltype(file)>(::Test::rand(UINT32_MAX)));
		std::string name(std::string("file") + std::to_string(::Test::rand(UINT32_MAX)));
		Entry entry(data, name, false);
		EntryAccess::getContent(entry).data = file;
		std::shared_ptr<Entry> pentry(
			&entry, [](Entry* e) { EntryAccess::getContent(*e).data = nullptr; });
		auto buffer = GenerateRandomBufferedContentState(EntryAccess::getContent(entry));
		auto offset = EntryAccess::getContent(entry).lastWrite;
		EXPECT_CALL(lib, fread(file, buffer.get() + offset, _)).WillOnce(ReturnArg<2>());
		char tmp[randomLenBase()];
		std::int64_t res;
		EXPECT_GE(randomLenBase(), res = entry.read(tmp, randomLenBase(), offset));
		EXPECT_EQ(SimpleSpan<dynamic_extend>(buffer.get() + offset, res).content(),
			SimpleSpan<dynamic_extend>(tmp, res).content());
	}

	TEST_F(EntryTest, ReadEnd)
	{
		using Fixtures::dynamic_extend;
		Fixtures::Lib lib;
		::ZipDirFs::Zip::Base::Lib::File* file(
			reinterpret_cast<decltype(file)>(::Test::rand(UINT32_MAX)));
		std::string name(std::string("file") + std::to_string(::Test::rand(UINT32_MAX)));
		Entry entry(data, name, false);
		EntryAccess::getContent(entry).data = file;
		std::shared_ptr<Entry> pentry(
			&entry, [](Entry* e) { EntryAccess::getContent(*e).data = nullptr; });
		auto buffer = GenerateRandomBufferedContentState(EntryAccess::getContent(entry));
		const auto len = randomLenBase() / 2;
		auto offset = EntryAccess::getContent(entry).lastWrite =
			EntryAccess::getContent(entry).length - len;
		::testing::ExpectationSet set;
		set += EXPECT_CALL(lib, fread(file, buffer.get() + offset, randomLenBase()))
				   .WillOnce(Return(len));
		EXPECT_CALL(lib, fclose(file)).After(set);
		char tmp[len];
		EXPECT_EQ(len, entry.read(tmp, len, offset));
		EXPECT_EQ(nullptr, EntryAccess::getContent(entry).data);
		EXPECT_EQ(SimpleSpan<dynamic_extend>(buffer.get() + offset, len).content(),
			SimpleSpan<dynamic_extend>(tmp, len).content());
	}

	TEST_F(EntryTest, ReadOverEnd)
	{
		Fixtures::Lib lib;
		::ZipDirFs::Zip::Base::Lib::File* file(
			reinterpret_cast<decltype(file)>(::Test::rand(UINT32_MAX)));
		std::string name(std::string("file") + std::to_string(::Test::rand(UINT32_MAX)));
		Entry entry(data, name, false);
		std::unique_ptr<char, std::function<void(char*)>> buffer(
			reinterpret_cast<char*>(::Test::rand(UINT32_MAX)),
			[&entry](char*) { EntryAccess::getContent(entry).buffer = nullptr; });
		EntryAccess::getContent(entry).buffer = buffer.get();
		EntryAccess::getContent(entry).lastWrite = EntryAccess::getContent(entry).length =
			::Test::rand(randomLenBase(), randomLenMax() - 1);
		auto offset = EntryAccess::getContent(entry).length;
		EXPECT_EQ(0, entry.read(nullptr, randomLenBase(), offset));
	}
} // namespace Test::ZipDirFs::Zip
