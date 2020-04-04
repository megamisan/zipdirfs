/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "Entry.h"
#include "test/gtest.h"
#include <boost/filesystem.hpp>
#include <csignal>
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
	using ::ZipDirFs::Zip::Entry;

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
	std::shared_ptr<Content> EntryAccess::invokeOpen(Entry& e)
	{
		return reinterpret_cast<EntryAccess*>(&e)->open();
	}
	void EntryAccess::registerArchiveAndEntry(
		const std::shared_ptr<::ZipDirFs::Zip::Archive>& a, const std::shared_ptr<Entry>& e)
	{
		ArchiveAccess::getNames(*a).emplace_back(EntryAccess::getCachedStat(*e).getName());
		ArchiveAccess::getNameAttributes(*a).emplace(EntryAccess::getCachedStat(*e).getName(),
			std::tuple<std::uint64_t, bool>{EntryAccess::getCachedStat(*e).getIndex(), false});
		ArchiveAccess::getEntries(*a).insert({EntryAccess::getCachedStat(*e).getIndex(), e});
		FactoryAccess::getArchivesByData().insert({EntryAccess::getData(*e).get(), a});
	}
	void EntryAccess::cleanupArchiveAndEntry() { FactoryAccess::getArchivesByData().clear(); }

	EntryAccess::GlobalHelper::GlobalHelper(
		Fixtures::Lib& l, const std::shared_ptr<::ZipDirFs::Zip::Entry>& e) :
		archiveInstance(EntryAccess::getData(*e).get()),
		archive(std::shared_ptr<::ZipDirFs::Zip::Archive>(
			&archiveInstance, [](::ZipDirFs::Zip::Archive*) {})),
		entry(e)
	{
		registerArchiveAndEntry(archive, entry);
		EXPECT_CALL(l, close(_));
	}
	EntryAccess::GlobalHelper::~GlobalHelper() { cleanupArchiveAndEntry(); }

	std::vector<std::string>& EntryAccess::ArchiveAccess::getNames(::ZipDirFs::Zip::Archive& a)
	{
		return reinterpret_cast<ArchiveAccess*>(&a)->names;
	}
	std::map<std::string, std::tuple<std::uint64_t, bool>>&
		EntryAccess::ArchiveAccess::getNameAttributes(::ZipDirFs::Zip::Archive& a)
	{
		return reinterpret_cast<ArchiveAccess*>(&a)->nameAttributes;
	}
	std::map<std::uint64_t, std::weak_ptr<::ZipDirFs::Zip::Entry>>&
		EntryAccess::ArchiveAccess::getEntries(::ZipDirFs::Zip::Archive& a)
	{
		return reinterpret_cast<ArchiveAccess*>(&a)->entries;
	}

	std::map<const ::ZipDirFs::Zip::Base::Lib*, std::weak_ptr<::ZipDirFs::Zip::Archive>>&
		EntryAccess::FactoryAccess::getArchivesByData()
	{
		return Factory::archivesByData;
	}

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
			[](int status) {
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
		EntryAccess::GlobalHelper helper(lib, std::shared_ptr<Entry>(&entry, [](Entry* e) {
			EntryAccess::getContent(*e).data = nullptr;
		}));
		auto content(EntryAccess::invokeOpen(entry));
		EXPECT_EQ(content.get(), &EntryAccess::getContent(entry));
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
		EntryAccess::GlobalHelper helper(lib, std::shared_ptr<Entry>(&entry, [](Entry*) {}));
		EXPECT_EQ(EntryAccess::invokeOpen(entry).get(), &EntryAccess::getContent(entry));
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
} // namespace Test::ZipDirFs::Zip
