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
		static std::shared_ptr<Content> invokeOpen(::ZipDirFs::Zip::Entry&);
		struct GlobalHelper
		{
			GlobalHelper(Fixtures::Lib&, const std::shared_ptr<::ZipDirFs::Zip::Entry>&);
			~GlobalHelper();

		protected:
			::ZipDirFs::Zip::Archive archiveInstance;
			const std::shared_ptr<::ZipDirFs::Zip::Archive> archive;
			const std::shared_ptr<Entry> entry;
		};

	protected:
		static void registerArchiveAndEntry(const std::shared_ptr<::ZipDirFs::Zip::Archive>&,
			const std::shared_ptr<::ZipDirFs::Zip::Entry>&);
		static void cleanupArchiveAndEntry();
		struct ArchiveAccess : public ::ZipDirFs::Zip::Archive
		{
			static std::vector<std::string>& getNames(::ZipDirFs::Zip::Archive&);
			static std::map<std::string, std::tuple<std::uint64_t, bool>>& getNameAttributes(
				::ZipDirFs::Zip::Archive&);
			static std::map<std::uint64_t, std::weak_ptr<::ZipDirFs::Zip::Entry>>& getEntries(
				::ZipDirFs::Zip::Archive&);
		};
		struct FactoryAccess : public ::ZipDirFs::Zip::Factory
		{
			static std::map<const ::ZipDirFs::Zip::Base::Lib*,
				std::weak_ptr<::ZipDirFs::Zip::Archive>>&
				getArchivesByData();
		};
	};

	struct EntryTest : public ::testing::Test
	{
		void SetUp();

	protected:
		std::shared_ptr<LibBase> data;
	};
} // namespace Test::ZipDirFs::Zip

#endif // TEST_ZIPDIRFS_ZIP_ENTRY_H
