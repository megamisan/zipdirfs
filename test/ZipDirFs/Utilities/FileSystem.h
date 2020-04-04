/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_UTILITIES_FILESYSTEM_H
#define TEST_ZIPDIRFS_UTILITIES_FILESYSTEM_H

#include "ZipDirFs/Containers/EntryIterator.h"
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

namespace Test::ZipDirFs::Utilities
{
	struct DirectoryIteratorWrapperAccess : public ::ZipDirFs::Containers::EntryIterator::Wrapper
	{
		static const boost::filesystem::directory_iterator& getDirIt(const ::ZipDirFs::Containers::EntryIterator::Wrapper*);
	protected:
		boost::filesystem::directory_iterator current_it;
	};

	class FileSystemTest : public ::testing::Test
	{
	public:
	};

} // namespace Test::ZipDirFs::Utilities

#endif // TEST_ZIPDIRFS_UTILITIES_FILESYSTEM_H
