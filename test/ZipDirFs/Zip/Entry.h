/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_ZIP_ENTRY_H
#define TEST_ZIPDIRFS_ZIP_ENTRY_H

#include "Fixtures/Lib.h"
#include "ZipDirFs/Zip/Archive.h"
#include "ZipDirFs/Zip/Entry.h"
#include "ZipDirFs/Zip/Factory.h"
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Zip
{
	using LibBase = ::ZipDirFs::Zip::Base::Lib;
	using ::ZipDirFs::Zip::Base::Content;
	using ::ZipDirFs::Zip::Base::Stat;

	struct EntryAccess : public ::ZipDirFs::Zip::Entry
	{
		static const std::shared_ptr<LibBase>& getData(::ZipDirFs::Zip::Entry&);
		static const std::string& getName(::ZipDirFs::Zip::Entry&);
		static std::bitset<2>& getFlags(::ZipDirFs::Zip::Entry&);
		static Stat& getCachedStat(::ZipDirFs::Zip::Entry&);
		static Content& getContent(::ZipDirFs::Zip::Entry&);
		static void invokeOpen(::ZipDirFs::Zip::Entry&);
	};

	struct EntryTest : public ::testing::Test
	{
		void SetUp();

	protected:
		std::shared_ptr<LibBase> data;
	};
} // namespace Test::ZipDirFs::Zip

#endif // TEST_ZIPDIRFS_ZIP_ENTRY_H
