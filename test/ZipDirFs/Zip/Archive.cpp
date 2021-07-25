/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "Archive.h"
#include "Factory.h"
#include "ZipDirFs/Zip/Factory.h"
#include "test/gtest.h"
#include <boost/filesystem.hpp>
#include <gmock/gmock.h>
#include <utility>

namespace ZipDirFs::Zip
{
	template <typename T>
	bool operator!=(const T& t1, const T& t2)
	{
		return !(t1 == t2);
	}
} // namespace ZipDirFs::Zip

namespace Test::ZipDirFs::Zip
{
	using BaseLib = ::ZipDirFs::Zip::Base::Lib;
	using ::testing::_;
	using ::testing::ContainerEq;
	using ::testing::InSequence;
	using ::testing::Return;
	using ::ZipDirFs::Zip::Archive;

	BaseLib* const& ArchiveAccess::getData(Archive& a)
	{
		return reinterpret_cast<ArchiveAccess*>(&a)->data;
	}
	std::vector<std::string>& ArchiveAccess::getNames(Archive& a)
	{
		return reinterpret_cast<ArchiveAccess*>(&a)->names;
	}
	std::map<std::string, std::tuple<std::uint64_t, bool>>& ArchiveAccess::getNameAttributes(
		Archive& a)
	{
		return reinterpret_cast<ArchiveAccess*>(&a)->nameAttributes;
	}
	std::map<std::uint64_t, std::weak_ptr<::ZipDirFs::Zip::Entry>>& ArchiveAccess::getEntries(
		Archive& a)
	{
		return reinterpret_cast<ArchiveAccess*>(&a)->entries;
	}
	void ArchiveAccess::invokePopulate(::ZipDirFs::Zip::Archive& a)
	{
		reinterpret_cast<ArchiveAccess*>(&a)->populate();
	}
	Archive::ArchiveIterator ArchiveIteratorAccess::create(const std::shared_ptr<Archive>& a,
		const std::shared_ptr<std::string>& p, std::vector<std::string>::iterator&& it)
	{
		ArchiveIteratorAccess aia(a, p, std::move(it));
		return Archive::ArchiveIterator(*static_cast<Archive::ArchiveIterator*>(&aia));
	}
	std::shared_ptr<Archive>& ArchiveIteratorAccess::getArchive(Archive::ArchiveIterator& ai)
	{
		return reinterpret_cast<ArchiveIteratorAccess*>(&ai)->archive;
	}
	std::shared_ptr<std::string>& ArchiveIteratorAccess::getPrefix(Archive::ArchiveIterator& ai)
	{
		return reinterpret_cast<ArchiveIteratorAccess*>(&ai)->prefix;
	}
	std::vector<std::string>::iterator& ArchiveIteratorAccess::getCurrent(
		Archive::ArchiveIterator& ai)
	{
		return reinterpret_cast<ArchiveIteratorAccess*>(&ai)->current;
	}
	std::string& ArchiveIteratorAccess::getFilename(Archive::ArchiveIterator& ai)
	{
		return reinterpret_cast<ArchiveIteratorAccess*>(&ai)->filename;
	}
	Archive::ArchiveIterator ArchiveIteratorAccess::make(const std::shared_ptr<Archive>& a)
	{
		ArchiveIteratorAccess aia(a);
		return Archive::ArchiveIterator(*static_cast<Archive::ArchiveIterator*>(&aia));
	}
	Archive::ArchiveIterator ArchiveIteratorAccess::make(
		const std::shared_ptr<Archive>& a, std::vector<std::string>::iterator&& it)
	{
		ArchiveIteratorAccess aia(a, std::move(it));
		return Archive::ArchiveIterator(*static_cast<Archive::ArchiveIterator*>(&aia));
	}
	Archive::ArchiveIterator ArchiveIteratorAccess::make(
		const std::shared_ptr<Archive>& a, const std::string& p)
	{
		ArchiveIteratorAccess aia(a, p);
		return Archive::ArchiveIterator(*static_cast<Archive::ArchiveIterator*>(&aia));
	}
	ArchiveIteratorAccess::ArchiveIteratorAccess(const std::shared_ptr<Archive>& a,
		const std::shared_ptr<std::string>& p, std::vector<std::string>::iterator it) :
		ArchiveIterator(a, std::move(std::vector<std::string>::iterator(it)))
	{
		archive = a;
		prefix = p;
		current = it;
	}
	ArchiveIteratorAccess::ArchiveIteratorAccess(const std::shared_ptr<Archive>& a) :
		ArchiveIterator(a)
	{
	}
	ArchiveIteratorAccess::ArchiveIteratorAccess(
		const std::shared_ptr<Archive>& a, std::vector<std::string>::iterator&& it) :
		ArchiveIterator(a, std::move(it))
	{
	}
	ArchiveIteratorAccess::ArchiveIteratorAccess(
		const std::shared_ptr<Archive>& a, const std::string& p) :
		ArchiveIterator(a, p)
	{
	}

	std::shared_ptr<Archive> ArchiveTest::CreateArchive(Fixtures::Lib& lib)
	{
		std::shared_ptr<Archive> archive(
			Archive::create(reinterpret_cast<BaseLib*>(::Test::rand(UINT32_MAX))));
		EXPECT_CALL(lib, close(_));
		return archive;
	}
	std::tuple<std::string, std::uint64_t> ArchiveTest::MakeEntry(
		const std::shared_ptr<Archive>& a, const std::string& n)
	{
		std::uint64_t index(::Test::rand(UINT32_MAX));
		ArchiveAccess::getNames(*a).emplace_back(n);
		ArchiveAccess::getNameAttributes(*a).emplace(
			n, std::tuple<std::uint64_t, bool>(index, false));
		return {n, index};
	}
	std::tuple<std::string, std::uint64_t, std::shared_ptr<::ZipDirFs::Zip::Entry>>
		ArchiveTest::CreateEntry(const std::shared_ptr<Archive>& a, const std::string& n)
	{
		auto built = MakeEntry(a, n);
		std::shared_ptr<::ZipDirFs::Zip::Entry> entry(
			reinterpret_cast<::ZipDirFs::Zip::Entry*>(::Test::rand(UINT32_MAX)),
			[](::ZipDirFs::Zip::Entry*) {});
		ArchiveAccess::getEntries(*a).insert({std::get<1>(built), entry});
		return {std::get<0>(built), std::get<1>(built), entry};
	}
	void ArchiveTest::MockNameSequence(
		Fixtures::Lib& lib, ::ZipDirFs::Zip::Archive& a, std::vector<std::string> list)
	{
		InSequence seq;
		auto const data = ArchiveAccess::getData(a);
		EXPECT_CALL(lib, get_num_entries(data)).WillOnce(Return(list.size()));
		for (std::uint64_t index = 0; index < list.size(); ++index)
			EXPECT_CALL(lib, get_name(data, index)).WillOnce(Return(list[index]));
	}

	std::shared_ptr<Archive> ArchiveIteratorTest::CreateArchive(Fixtures::Lib& lib, int times)
	{
		std::shared_ptr<Archive> archive(
			Archive::create(reinterpret_cast<BaseLib*>(::Test::rand(UINT32_MAX))));
		if (times)
			EXPECT_CALL(lib, close(_)).Times(times);
		return archive;
	}
	void ArchiveIteratorTest::InitializeNames(Archive& a, std::vector<std::string>&& ns)
	{
		ArchiveAccess::getNames(a) = std::move(ns);
	}

	std::string ArchiveGenerateValue(std::uint8_t length)
	{
		std::string result;
		do
		{
			result = std::to_string(::Test::rand(UINT32_MAX));
		} while (result.length() < length);
		return result;
	}

	std::string ArchiveGeneratePrefix(std::uint8_t level, std::uint8_t segmentLength = 0)
	{
		std::string path;
		if (level > 0)
		{
			auto len = segmentLength ? segmentLength : (::Test::rand(UINT32_MAX) % 9 + 1);
			std::string segment(ArchiveGenerateValue(len));
			path = "seg" + segment.substr(0, len) + '/'
				+ ArchiveGeneratePrefix(level - 1, segmentLength);
		}
		return path;
	}

	std::string ArchiveGeneratePath(std::uint8_t level, bool dir, std::uint8_t segmentLength = 0)
	{
		std::string path(ArchiveGeneratePrefix(level, segmentLength)
			+ std::string(dir ? "folder" : "file") + std::to_string(::Test::rand(UINT32_MAX)));
		return path;
	}

	TEST_F(ArchiveTest, Instantiate)
	{
		Fixtures::Lib lib;
		EXPECT_CALL(lib, close(_));
		BaseLib* const opened = reinterpret_cast<decltype(opened)>(::Test::rand(UINT32_MAX));
		auto archive = Archive::create(opened);
		EXPECT_EQ(ArchiveAccess::getData(*archive), opened);
		EXPECT_TRUE(ArchiveAccess::getNames(*archive).empty());
		EXPECT_TRUE(ArchiveAccess::getNameAttributes(*archive).empty());
		EXPECT_TRUE(ArchiveAccess::getEntries(*archive).empty());
	}

	TEST_F(ArchiveTest, Destroy)
	{
		Fixtures::Lib lib;
		BaseLib* const opened = reinterpret_cast<decltype(opened)>(::Test::rand(UINT32_MAX));
		EXPECT_CALL(lib, close(opened));
		Archive::create(opened);
	}

	TEST_F(ArchiveTest, BeginNoPath)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		ArchiveAccess::getNames(*archive).insert(
			ArchiveAccess::getNames(*archive).end(), ArchiveGeneratePath(0, false));
		auto it = archive->begin();
		EXPECT_EQ(ArchiveIteratorAccess::getArchive(it), archive);
		EXPECT_EQ(ArchiveIteratorAccess::getCurrent(it), ArchiveAccess::getNames(*archive).begin());
		ASSERT_NE(ArchiveIteratorAccess::getPrefix(it), nullptr);
		EXPECT_EQ(*ArchiveIteratorAccess::getPrefix(it), "");
	}

	TEST_F(ArchiveTest, BeginPath)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		std::string prefix(ArchiveGeneratePrefix(1));
		ArchiveAccess::getNames(*archive).insert(
			ArchiveAccess::getNames(*archive).end(), prefix + ArchiveGeneratePath(0, false));
		auto it = archive->begin(prefix);
		EXPECT_EQ(ArchiveIteratorAccess::getArchive(it), archive);
		EXPECT_EQ(ArchiveIteratorAccess::getCurrent(it), ArchiveAccess::getNames(*archive).begin());
		ASSERT_NE(ArchiveIteratorAccess::getPrefix(it), nullptr);
		EXPECT_EQ(*ArchiveIteratorAccess::getPrefix(it), prefix);
	}

	TEST_F(ArchiveTest, End)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		ArchiveAccess::getNames(*archive).insert(
			ArchiveAccess::getNames(*archive).end(), ArchiveGeneratePath(1, false));
		auto it = archive->end();
		EXPECT_EQ(ArchiveIteratorAccess::getArchive(it), archive);
		EXPECT_EQ(ArchiveIteratorAccess::getCurrent(it), ArchiveAccess::getNames(*archive).end());
		EXPECT_EQ(ArchiveIteratorAccess::getPrefix(it), nullptr);
	}

	TEST_F(ArchiveTest, OpenCreate)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		auto info(MakeEntry(archive, ArchiveGeneratePath(1, false)));
		auto entry(archive->open(std::get<0>(info)));
		ASSERT_NE(ArchiveAccess::getEntries(*archive).find(std::get<1>(info)),
			ArchiveAccess::getEntries(*archive).end());
		EXPECT_EQ(
			ArchiveAccess::getEntries(*archive).find(std::get<1>(info))->second.lock(), entry);
	}

	TEST_F(ArchiveTest, OpenUse)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		auto info(CreateEntry(archive, ArchiveGeneratePath(1, false)));
		EXPECT_EQ(archive->open(std::get<0>(info)), std::get<2>(info));
	}

	TEST_F(ArchiveTest, OpenExpired)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		auto info(CreateEntry(archive, ArchiveGeneratePath(1, false)));
		auto entryPtr = std::get<2>(info).get();
		std::get<2>(info) = nullptr;
		auto entry = archive->open(std::get<0>(info));
		EXPECT_NE(entry.get(), entryPtr);
		ASSERT_NE(ArchiveAccess::getEntries(*archive).find(std::get<1>(info)),
			ArchiveAccess::getEntries(*archive).end());
		EXPECT_NE(ArchiveAccess::getEntries(*archive).find(std::get<1>(info))->second.lock().get(),
			entryPtr);
		EXPECT_EQ(
			ArchiveAccess::getEntries(*archive).find(std::get<1>(info))->second.lock(), entry);
	}

	TEST_F(ArchiveTest, PopulateBase)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		std::vector<std::string> values = {
			ArchiveGeneratePath(0, false), ArchiveGeneratePath(0, false)};
		MockNameSequence(lib, *archive, values);
		ArchiveAccess::invokePopulate(*archive);
		EXPECT_THAT(ArchiveAccess::getNames(*archive), ContainerEq(values));
		EXPECT_THAT(ArchiveAccess::getNameAttributes(*archive),
			ContainerEq(std::map<std::string, std::tuple<std::uint64_t, bool>>{
				{values[0], {0, false}}, {values[1], {1, false}}}));
	}

	TEST_F(ArchiveTest, PopulateOneFolder)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		std::string prefix = ArchiveGeneratePrefix(1);
		std::vector<std::string> values = {
			prefix + ArchiveGeneratePath(0, false), prefix + ArchiveGeneratePath(0, false)};
		MockNameSequence(lib, *archive, values);
		ArchiveAccess::invokePopulate(*archive);
		EXPECT_THAT(ArchiveAccess::getNames(*archive),
			ContainerEq(std::vector<std::string>{
				prefix.substr(0, prefix.length() - 1), values[0], values[1]}));
		EXPECT_THAT(ArchiveAccess::getNameAttributes(*archive),
			ContainerEq(std::map<std::string, std::tuple<std::uint64_t, bool>>{
				{prefix.substr(0, prefix.length() - 1), {-1ULL, true}}, {values[0], {0, false}},
				{values[1], {1, false}}}));
	}

	TEST_F(ArchiveTest, PopulateTwoFolders)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		std::string prefixes[2] = {ArchiveGeneratePrefix(1), ArchiveGeneratePrefix(1)};
		std::vector<std::string> values = {
			prefixes[0] + prefixes[1] + ArchiveGeneratePath(0, false),
			prefixes[1] + ArchiveGeneratePath(0, false)};
		MockNameSequence(lib, *archive, values);
		ArchiveAccess::invokePopulate(*archive);
		EXPECT_THAT(ArchiveAccess::getNames(*archive),
			ContainerEq(std::vector<std::string>{prefixes[0].substr(0, prefixes[0].length() - 1),
				prefixes[0] + prefixes[1].substr(0, prefixes[1].length() - 1), values[0],
				prefixes[1].substr(0, prefixes[1].length() - 1), values[1]}));
		EXPECT_THAT(ArchiveAccess::getNameAttributes(*archive),
			ContainerEq(std::map<std::string, std::tuple<std::uint64_t, bool>>{
				{prefixes[0].substr(0, prefixes[0].length() - 1), {-1ULL, true}},
				{prefixes[0] + prefixes[1].substr(0, prefixes[1].length() - 1), {-2ULL, true}},
				{prefixes[1].substr(0, prefixes[1].length() - 1), {-3ULL, true}},
				{values[0], {0, false}}, {values[1], {1, false}}}));
	}

	TEST_F(ArchiveIteratorTest, Dereference)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		Archive::ArchiveIterator iterator(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string("")),
			ArchiveAccess::getNames(*archive).end()));
		std::string filename(ArchiveGeneratePath(0, false));
		ArchiveIteratorAccess::getFilename(iterator) = filename;
		EXPECT_EQ(*iterator, filename);
	}

	TEST_F(ArchiveIteratorTest, IncrementNextValidFileNoPrefix)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		const std::string expected(ArchiveGeneratePath(0, false));
		InitializeNames(
			*archive, std::vector<std::string>{ArchiveGeneratePath(0, false), expected});
		Archive::ArchiveIterator iterator(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string("")),
			ArchiveAccess::getNames(*archive).begin()));
		++iterator;
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator), expected);
		ASSERT_NE(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).end());
		EXPECT_EQ(*ArchiveIteratorAccess::getCurrent(iterator), expected);
	}

	TEST_F(ArchiveIteratorTest, IncrementNextValidFolderNoPrefix)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		const std::string expected(ArchiveGeneratePath(0, true));
		InitializeNames(
			*archive, std::vector<std::string>{ArchiveGeneratePath(0, false), expected});
		Archive::ArchiveIterator iterator(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string("")),
			ArchiveAccess::getNames(*archive).begin()));
		++iterator;
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator), expected);
		ASSERT_NE(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).end());
		EXPECT_EQ(*ArchiveIteratorAccess::getCurrent(iterator), expected);
	}

	TEST_F(ArchiveIteratorTest, IncrementNextValidFileWithPrefix)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		const std::string expectedFilename(ArchiveGeneratePath(0, false)),
			prefix(ArchiveGeneratePrefix(1)), expected(prefix + expectedFilename);
		InitializeNames(
			*archive, std::vector<std::string>{ArchiveGeneratePath(0, false), expected});
		Archive::ArchiveIterator iterator(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string(prefix)),
			ArchiveAccess::getNames(*archive).begin()));
		++iterator;
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator), expectedFilename);
		ASSERT_NE(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).end());
		EXPECT_EQ(*ArchiveIteratorAccess::getCurrent(iterator), expected);
	}

	TEST_F(ArchiveIteratorTest, IncrementNextValidFolderWithPrefix)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		const std::string expectedFilename(ArchiveGeneratePath(0, true)),
			prefix(ArchiveGeneratePrefix(1)), expected(prefix + expectedFilename);
		InitializeNames(
			*archive, std::vector<std::string>{ArchiveGeneratePath(0, false), expected});
		Archive::ArchiveIterator iterator(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string(prefix)),
			ArchiveAccess::getNames(*archive).begin()));
		++iterator;
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator), expectedFilename);
		ASSERT_NE(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).end());
		EXPECT_EQ(*ArchiveIteratorAccess::getCurrent(iterator), expected);
	}

	TEST_F(ArchiveIteratorTest, IncrementNextSkipFileNoPrefix)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		const std::string expected(ArchiveGeneratePath(0, false));
		InitializeNames(*archive,
			std::vector<std::string>{
				ArchiveGeneratePath(0, false), ArchiveGeneratePath(1, false), expected});
		Archive::ArchiveIterator iterator(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string("")),
			ArchiveAccess::getNames(*archive).begin()));
		++iterator;
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator), expected);
		ASSERT_NE(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).end());
		EXPECT_EQ(*ArchiveIteratorAccess::getCurrent(iterator), expected);
	}

	TEST_F(ArchiveIteratorTest, IncrementNextSkipFolderNoPrefix)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		const std::string expected(ArchiveGeneratePath(0, false));
		InitializeNames(*archive,
			std::vector<std::string>{
				ArchiveGeneratePath(0, false), ArchiveGeneratePath(1, true), expected});
		Archive::ArchiveIterator iterator(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string("")),
			ArchiveAccess::getNames(*archive).begin()));
		++iterator;
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator), expected);
		ASSERT_NE(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).end());
		EXPECT_EQ(*ArchiveIteratorAccess::getCurrent(iterator), expected);
	}

	TEST_F(ArchiveIteratorTest, IncrementNextSkipFileShorterPrefix)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		const std::string prefix(ArchiveGeneratePrefix(1)),
			expectedFilename(ArchiveGeneratePath(0, false)), expected(prefix + expectedFilename);
		InitializeNames(*archive,
			std::vector<std::string>{
				prefix + ArchiveGeneratePath(0, false), ArchiveGeneratePath(0, false), expected});
		Archive::ArchiveIterator iterator(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string(prefix)),
			ArchiveAccess::getNames(*archive).begin()));
		++iterator;
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator), expectedFilename);
		ASSERT_NE(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).end());
		EXPECT_EQ(*ArchiveIteratorAccess::getCurrent(iterator), expected);
	}

	TEST_F(ArchiveIteratorTest, IncrementNextSkipFolderShorterPrefix)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		const std::string prefix(ArchiveGeneratePrefix(1)),
			expectedFilename(ArchiveGeneratePath(0, false)), expected(prefix + expectedFilename);
		InitializeNames(*archive,
			std::vector<std::string>{
				prefix + ArchiveGeneratePath(0, false), ArchiveGeneratePath(0, true), expected});
		Archive::ArchiveIterator iterator(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string(prefix)),
			ArchiveAccess::getNames(*archive).begin()));
		++iterator;
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator), expectedFilename);
		ASSERT_NE(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).end());
		EXPECT_EQ(*ArchiveIteratorAccess::getCurrent(iterator), expected);
	}

	TEST_F(ArchiveIteratorTest, IncrementNextSkipFileLongerPrefix)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		const std::string prefix(ArchiveGeneratePrefix(1)),
			expectedFilename(ArchiveGeneratePath(0, false)), expected(prefix + expectedFilename);
		InitializeNames(*archive,
			std::vector<std::string>{prefix + ArchiveGeneratePath(0, false),
				prefix + ArchiveGeneratePath(1, false), expected});
		Archive::ArchiveIterator iterator(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string(prefix)),
			ArchiveAccess::getNames(*archive).begin()));
		++iterator;
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator), expectedFilename);
		ASSERT_NE(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).end());
		EXPECT_EQ(*ArchiveIteratorAccess::getCurrent(iterator), expected);
	}

	TEST_F(ArchiveIteratorTest, IncrementNextSkipFolderLongerPrefix)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		const std::string prefix(ArchiveGeneratePrefix(1)),
			expectedFilename(ArchiveGeneratePath(0, false)), expected(prefix + expectedFilename);
		InitializeNames(*archive,
			std::vector<std::string>{prefix + ArchiveGeneratePath(0, false),
				prefix + ArchiveGeneratePath(1, true), expected});
		Archive::ArchiveIterator iterator(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string(prefix)),
			ArchiveAccess::getNames(*archive).begin()));
		++iterator;
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator), expectedFilename);
		ASSERT_NE(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).end());
		EXPECT_EQ(*ArchiveIteratorAccess::getCurrent(iterator), expected);
	}

	TEST_F(ArchiveIteratorTest, IncrementNextSkipFileSameLengthPrefix)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		const std::uint8_t segmentLength = ::Test::rand(1, 9);
		const std::string firstSegment(
			"seg" + ArchiveGenerateValue(segmentLength).substr(0, segmentLength)),
			expectedSegment(
				[&firstSegment](std::uint8_t segmentLength)
				{
					std::string result;
					do
					{
						result =
							"seg" + ArchiveGenerateValue(segmentLength).substr(0, segmentLength);
					} while (result == firstSegment);
					return result;
				}(segmentLength)),
			expectedFilename(ArchiveGeneratePath(0, false)),
			expected(expectedSegment + "/" + expectedFilename);
		InitializeNames(*archive,
			std::vector<std::string>{expectedSegment + "/" + ArchiveGeneratePath(0, false),
				firstSegment + '/' + ArchiveGeneratePath(0, false), expected});
		Archive::ArchiveIterator iterator(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string(expectedSegment + "/")),
			ArchiveAccess::getNames(*archive).begin()));
		++iterator;
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator), expectedFilename);
		ASSERT_NE(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).end());
		EXPECT_EQ(*ArchiveIteratorAccess::getCurrent(iterator), expected);
	}

	TEST_F(ArchiveIteratorTest, IncrementNextSkipFolderSameLengthPrefix)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		const std::uint8_t segmentLength = ::Test::rand(1, 9);
		const std::string firstSegment(
			"seg" + ArchiveGenerateValue(segmentLength).substr(0, segmentLength)),
			expectedSegment(
				[&firstSegment](std::uint8_t segmentLength)
				{
					std::string result;
					do
					{
						result =
							"seg" + ArchiveGenerateValue(segmentLength).substr(0, segmentLength);
					} while (result == firstSegment);
					return result;
				}(segmentLength)),
			expectedFilename(ArchiveGeneratePath(0, false)),
			expected(expectedSegment + "/" + expectedFilename);
		InitializeNames(*archive,
			std::vector<std::string>{expectedSegment + "/" + ArchiveGeneratePath(0, false),
				firstSegment + '/' + ArchiveGeneratePath(0, true), expected});
		Archive::ArchiveIterator iterator(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string(expectedSegment + "/")),
			ArchiveAccess::getNames(*archive).begin()));
		++iterator;
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator), expectedFilename);
		ASSERT_NE(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).end());
		EXPECT_EQ(*ArchiveIteratorAccess::getCurrent(iterator), expected);
	}

	TEST_F(ArchiveIteratorTest, EqualSame)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		InitializeNames(*archive, std::vector<std::string>{"first", "second"});
		Archive::ArchiveIterator iterator1(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string("")),
			ArchiveAccess::getNames(*archive).begin())),
			iterator2(ArchiveIteratorAccess::create(archive,
				std::shared_ptr<std::string>(new std::string("")),
				ArchiveAccess::getNames(*archive).begin()));
		EXPECT_EQ(iterator2, iterator1);
	}

	TEST_F(ArchiveIteratorTest, NotEqualPositionDifferent)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		InitializeNames(*archive, std::vector<std::string>{"first", "second"});
		auto second(++ArchiveAccess::getNames(*archive).begin());
		Archive::ArchiveIterator iterator1(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string("")),
			ArchiveAccess::getNames(*archive).begin())),
			iterator2(ArchiveIteratorAccess::create(
				archive, std::shared_ptr<std::string>(new std::string("")), std::move(second)));
		EXPECT_NE(iterator2, iterator1);
	}

	TEST_F(ArchiveIteratorTest, EqualEndLeft)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		Archive::ArchiveIterator iterator1(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(nullptr), ArchiveAccess::getNames(*archive).end())),
			iterator2(ArchiveIteratorAccess::create(archive,
				std::shared_ptr<std::string>(new std::string("")),
				ArchiveAccess::getNames(*archive).end()));
		EXPECT_EQ(iterator2, iterator1);
	}

	TEST_F(ArchiveIteratorTest, NotEqualEndLeft)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		InitializeNames(*archive, std::vector<std::string>{"first", "second"});
		Archive::ArchiveIterator iterator1(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string("")),
			ArchiveAccess::getNames(*archive).end())),
			iterator2(ArchiveIteratorAccess::create(archive,
				std::shared_ptr<std::string>(new std::string("")),
				ArchiveAccess::getNames(*archive).begin()));
		EXPECT_NE(iterator2, iterator1);
	}

	TEST_F(ArchiveIteratorTest, EqualEndRight)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		Archive::ArchiveIterator iterator1(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string("")),
			ArchiveAccess::getNames(*archive).end())),
			iterator2(ArchiveIteratorAccess::create(archive, std::shared_ptr<std::string>(nullptr),
				ArchiveAccess::getNames(*archive).end()));
		EXPECT_EQ(iterator2, iterator1);
	}

	TEST_F(ArchiveIteratorTest, NotEqualEndRight)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		InitializeNames(*archive, std::vector<std::string>{"first", "second"});
		Archive::ArchiveIterator iterator1(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(new std::string("")),
			ArchiveAccess::getNames(*archive).begin())),
			iterator2(ArchiveIteratorAccess::create(archive,
				std::shared_ptr<std::string>(new std::string("")),
				ArchiveAccess::getNames(*archive).end()));
		EXPECT_NE(iterator2, iterator1);
	}

	TEST_F(ArchiveIteratorTest, NotEqualPrefixDifferent)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		Archive::ArchiveIterator iterator1(ArchiveIteratorAccess::create(archive,
			std::shared_ptr<std::string>(
				new std::string(ArchiveGeneratePath(::Test::rand(4), false))),
			ArchiveAccess::getNames(*archive).end())),
			iterator2(ArchiveIteratorAccess::create(archive,
				std::shared_ptr<std::string>(
					new std::string(ArchiveGeneratePath(::Test::rand(4), false))),
				ArchiveAccess::getNames(*archive).end()));
		EXPECT_NE(iterator2, iterator1);
	}

	TEST_F(ArchiveIteratorTest, NotEqualArchiveDifferent)
	{
		Fixtures::Lib lib;
		auto archive1(CreateArchive(lib, 2)), archive2(CreateArchive(lib, 0));
		Archive::ArchiveIterator iterator1(ArchiveIteratorAccess::create(archive1,
			std::shared_ptr<std::string>(new std::string("")),
			ArchiveAccess::getNames(*archive1).end())),
			iterator2(ArchiveIteratorAccess::create(archive2,
				std::shared_ptr<std::string>(new std::string("")),
				ArchiveAccess::getNames(*archive2).end()));
		EXPECT_NE(iterator2, iterator1);
	}

	TEST_F(ArchiveIteratorTest, Swap)
	{
		Fixtures::Lib lib;
		auto archive1(CreateArchive(lib, 2)), archive2(CreateArchive(lib, 0));
		auto prefix1(ArchiveGeneratePrefix(::Test::rand(4))),
			prefix2(ArchiveGeneratePrefix(::Test::rand(4)));
		auto it1(ArchiveAccess::getNames(*archive1).begin()),
			it2(ArchiveAccess::getNames(*archive2).begin());
		auto filename1(ArchiveGeneratePath(0, false)), filename2(ArchiveGeneratePath(0, false));
		Archive::ArchiveIterator iterator1(ArchiveIteratorAccess::create(archive1,
			std::shared_ptr<std::string>(new std::string(prefix1)),
			std::move(std::vector<std::string>::iterator(it1)))),
			iterator2(ArchiveIteratorAccess::create(archive2,
				std::shared_ptr<std::string>(new std::string(prefix2)),
				std::move(std::vector<std::string>::iterator(it2))));
		ArchiveIteratorAccess::getFilename(iterator1) = filename1;
		ArchiveIteratorAccess::getFilename(iterator2) = filename2;
		std::swap(iterator2, iterator1);
		EXPECT_EQ(ArchiveIteratorAccess::getArchive(iterator1), archive2);
		EXPECT_EQ(*ArchiveIteratorAccess::getPrefix(iterator1), prefix2);
		EXPECT_EQ(ArchiveIteratorAccess::getCurrent(iterator1), it2);
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator1), filename2);
		EXPECT_EQ(ArchiveIteratorAccess::getArchive(iterator2), archive1);
		EXPECT_EQ(*ArchiveIteratorAccess::getPrefix(iterator2), prefix1);
		EXPECT_EQ(ArchiveIteratorAccess::getCurrent(iterator2), it1);
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator2), filename1);
	}

	TEST_F(ArchiveIteratorTest, InstantiateNoPrefix)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		std::string filename(ArchiveGeneratePath(0, false));
		InitializeNames(*archive, std::vector<std::string>({filename}));
		auto iterator(ArchiveIteratorAccess::make(archive));
		EXPECT_EQ(ArchiveIteratorAccess::getArchive(iterator), archive);
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator), filename);
		EXPECT_EQ(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).begin());
		ASSERT_NE(ArchiveIteratorAccess::getPrefix(iterator), nullptr);
		EXPECT_EQ(*ArchiveIteratorAccess::getPrefix(iterator), "");
	}

	TEST_F(ArchiveIteratorTest, InstantiateWithIterator)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		auto iterator(
			ArchiveIteratorAccess::make(archive, ArchiveAccess::getNames(*archive).end()));
		EXPECT_EQ(ArchiveIteratorAccess::getArchive(iterator), archive);
		EXPECT_EQ(ArchiveIteratorAccess::getPrefix(iterator), nullptr);
		EXPECT_EQ(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).end());
	}

	TEST_F(ArchiveIteratorTest, InstantiateWithPrefix)
	{
		Fixtures::Lib lib;
		auto archive(CreateArchive(lib));
		std::string prefix(ArchiveGeneratePrefix(1));
		std::string filename(ArchiveGeneratePath(0, false));
		InitializeNames(*archive, std::vector<std::string>({prefix + filename}));
		auto iterator(ArchiveIteratorAccess::make(archive, prefix));
		EXPECT_EQ(ArchiveIteratorAccess::getArchive(iterator), archive);
		EXPECT_EQ(ArchiveIteratorAccess::getFilename(iterator), filename);
		EXPECT_EQ(
			ArchiveIteratorAccess::getCurrent(iterator), ArchiveAccess::getNames(*archive).begin());
		ASSERT_NE(ArchiveIteratorAccess::getPrefix(iterator), nullptr);
		EXPECT_EQ(*ArchiveIteratorAccess::getPrefix(iterator), prefix);
	}
} // namespace Test::ZipDirFs::Zip
