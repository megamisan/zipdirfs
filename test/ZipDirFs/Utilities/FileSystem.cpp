/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "FileSystem.h"
#include "ZipDirFs/Utilities/FileSystem.h"
#include "test/gtest.h"
#include <boost/filesystem.hpp>
#include <ctime>
#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <list>
#include <map>
#include <vector>

namespace Test::ZipDirFs::Utilities
{
	using ::ZipDirFs::Utilities::FileSystem;
	using namespace ::boost;
	using ::testing::Contains;

	const filesystem::directory_iterator& DirectoryIteratorWrapperAccess::getDirIt(
		const ::ZipDirFs::Containers::EntryIterator::Wrapper* w)
	{
		return reinterpret_cast<const DirectoryIteratorWrapperAccess*>(w)->current_it;
	}

	namespace
	{
		filesystem::path tempFolderPath()
		{
			return filesystem::temp_directory_path()
				/ filesystem::unique_path("test-%%%%-%%%%-%%%%-%%%%");
		}

		filesystem::path tempItem() { return filesystem::unique_path("item-%%%%-%%%%"); }

		inline std::time_t getRandTime() { return ::Test::rand(631152000L, 3221225471); }

		struct GeneratedFolder
		{
			GeneratedFolder();
			~GeneratedFolder();
			constexpr const filesystem::path& getRoot() const { return root; }
			constexpr const std::vector<filesystem::path>& getItems() const { return items; }
			constexpr const std::map<filesystem::path, std::time_t>& getTimes() const
			{
				return times;
			}

		protected:
			filesystem::path root;
			std::vector<filesystem::path> items;
			std::map<filesystem::path, std::time_t> times;
		};

		struct Guard
		{
			Guard(std::function<void()> f) : _f(f){};
			~Guard() { _f(); }

		protected:
			std::function<void()> _f;
		};
	} // namespace

	GeneratedFolder::GeneratedFolder()
	{
		root = tempFolderPath();
		filesystem::create_directory(root);
		std::size_t count = ::Test::rand(2, 5);
		items.reserve(count);
		for (std::size_t i = 0; i < count; i++)
		{
			auto item = tempItem();
			items.push_back(item);
			filesystem::ofstream(root / item);
			filesystem::last_write_time(root / item, times[item] = getRandTime());
		}
	}

	GeneratedFolder::~GeneratedFolder()
	{
		for (auto item : items)
		{
			filesystem::remove(root / item);
		}
		EXPECT_NO_THROW(filesystem::remove(root));
	}

	TEST_F(FileSystemTest, LastWriteTime)
	{
		const auto expectedTime = getRandTime();
		const auto path = tempFolderPath();
		Guard rm([path]() { filesystem::remove(path); });
		filesystem::ofstream{path};
		filesystem::last_write_time(path, expectedTime);
		EXPECT_EQ(FileSystem::last_write_time(path), expectedTime);
	}

	TEST_F(FileSystemTest, DirItFromPath)
	{
		GeneratedFolder gf;
		auto it = FileSystem::directory_iterator_from_path(gf.getRoot());
		std::list<std::string> items;
		for (auto item : gf.getItems())
		{
			items.push_back(item.native());
		}
		std::size_t current = 0;
		while (items.size() != 0)
		{
			ASSERT_NE(DirectoryIteratorWrapperAccess::getDirIt(it.getWrapper()),
				filesystem::directory_iterator());
			EXPECT_THAT(items, Contains(*it)) << "  Current iteration: " << current;
			items.remove(*it);
			++it;
			++current;
		}
		EXPECT_EQ(DirectoryIteratorWrapperAccess::getDirIt(it.getWrapper()),
			filesystem::directory_iterator());
	}

	TEST_F(FileSystemTest, DirItEnd)
	{
		EXPECT_EQ(DirectoryIteratorWrapperAccess::getDirIt(
					  FileSystem::directory_iterator_end().getWrapper()),
			filesystem::directory_iterator());
	}

	TEST_F(FileSystemTest, DirItEquals)
	{
		auto expected = FileSystem::directory_iterator_end(), result = expected;
		EXPECT_EQ(result, expected);
	}

	TEST_F(FileSystemTest, DirItNotEquals)
	{
		GeneratedFolder gf;
		EXPECT_FALSE(FileSystem::directory_iterator_from_path(gf.getRoot())
						 .
					 operator==(FileSystem::directory_iterator_end()));
	}

	TEST_F(FileSystemTest, Status)
	{
		const auto path = tempFolderPath();
		Guard rm([path]() { filesystem::remove(path); });
		filesystem::ofstream{path};
		filesystem::file_status expected(filesystem::regular_file,
			filesystem::owner_read | filesystem::owner_write | filesystem::group_read
				| filesystem::others_read);
		EXPECT_EQ(FileSystem::status(path), expected);
	}
} // namespace Test::ZipDirFs::Utilities
