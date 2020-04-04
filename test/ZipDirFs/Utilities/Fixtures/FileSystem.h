/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_UTILITIES_FIXTURES_FILESYSTEM_H
#define TEST_ZIPDIRFS_UTILITIES_FIXTURES_FILESYSTEM_H

#include "ZipDirFs/Utilities/FileSystem.h"
#include <boost/filesystem.hpp>
#include <gmock/gmock.h>

namespace Test::ZipDirFs::Utilities
{
	namespace Fixtures
	{
		using namespace boost::filesystem;
		using ::ZipDirFs::Containers::EntryIterator;
		class FileSystem
		{
		public:
			FileSystem();
			~FileSystem();
			MOCK_METHOD1(last_write_time, std::time_t(const path&));
			MOCK_METHOD1(
				directory_iterator_from_path, EntryIterator::Wrapper*(const path&));
			MOCK_METHOD0(directory_iterator_end, EntryIterator::Wrapper*());
			MOCK_METHOD1(status, file_status(const path&));
		};

	} // namespace Fixtures
} // namespace Test::ZipDirFs::Utilities

#endif // TEST_ZIPDIRFS_UTILITIES_FIXTURES_FILESYSTEM_H
