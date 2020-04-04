/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_COMPONENTS_ZIPDIRECTORYENUMERATOR_H
#define TEST_ZIPDIRFS_COMPONENTS_ZIPDIRECTORYENUMERATOR_H

#include <gtest/gtest.h>

namespace Test::ZipDirFs::Components
{
	struct ZipDirectoryEnumeratorTest : public ::testing::Test
	{
		void TearDown();
	};
} // namespace Test::ZipDirFs::Components

#endif // TEST_ZIPDIRFS_COMPONENTS_ZIPDIRECTORYENUMERATOR_H
