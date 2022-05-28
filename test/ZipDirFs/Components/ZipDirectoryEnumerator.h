/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_COMPONENTS_ZIPDIRECTORYENUMERATOR_H
#define TEST_ZIPDIRFS_COMPONENTS_ZIPDIRECTORYENUMERATOR_H

#include "ZipDirFs/Components/ZipDirectoryEnumerator.h"
#include <gtest/gtest.h>

namespace ZipDirFs::Components
{
	extern std::time_t zipDirectoryEnumeratorDelay;
}

namespace Test::ZipDirFs::Components
{
	using ZipDirectoryEnumeratorBase = ::ZipDirFs::Components::ZipDirectoryEnumerator;

	struct ZipDirectoryEnumeratorAccess : public ZipDirectoryEnumeratorBase {
		const ::boost::filesystem::path& get_path() { return this->path; }
		const std::string& get_item() { return this->item; }
		const std::unique_ptr<HolderBase>& get_holder() { return this->holder; }
		const bool& get_atEnd() { return this->atEnd; }
	};

	struct ZipDirectoryEnumeratorTest : public ::testing::Test
	{
		void TearDown();
	};
} // namespace Test::ZipDirFs::Components

#endif // TEST_ZIPDIRFS_COMPONENTS_ZIPDIRECTORYENUMERATOR_H
