/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_COMPONENTS_ZIPFILECHANGED_H
#define TEST_ZIPDIRFS_COMPONENTS_ZIPFILECHANGED_H

#include "ZipDirFs/Components/ZipFileChanged.h"
#include "ZipDirFs/Zip/Factory.h"
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Components
{
	using ZipFileChangedBase = ::ZipDirFs::Components::ZipFileChanged;

	struct ZipFileChangedAccess : public ZipFileChangedBase
	{
		void set_lastPathChanged(std::time_t t) { this->lastPathChanged = t; }
		void set_lastItemChanged(std::time_t t) { this->lastItemChanged = t; }
		void set_state(std::bitset<2> v) { this->state = v; }
	};

	struct ZipFileChangedTest : public ::testing::Test
	{
		void TearDown();
	};
} // namespace Test::ZipDirFs::Components

#endif // TEST_ZIPDIRFS_COMPONENTS_ZIPFILECHANGED_H
