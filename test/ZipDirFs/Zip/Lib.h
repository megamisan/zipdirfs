/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_ZIP_LIB_H
#define TEST_ZIPDIRFS_ZIP_LIB_H

#include "ZipDirFs/Zip/Base/Lib.h"
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Zip
{
	class LibTest : public ::testing::Test
	{
	public:
		void SetUp();
		void TearDown();

	protected:
		::ZipDirFs::Zip::Base::Lib* libData;
		::ZipDirFs::Zip::Base::Lib::File* fileData;
	};

} // namespace Test::ZipDirFs::Zip

#endif // TEST_ZIPDIRFS_ZIP_LIB_H
