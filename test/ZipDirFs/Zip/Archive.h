/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_ZIP_ARCHIVE_H
#define TEST_ZIPDIRFS_ZIP_ARCHIVE_H

#include "Fixtures/Lib.h"
#include "ZipDirFs/Zip/Archive.h"
#include "ZipDirFs/Zip/Factory.h"
#include <functional>
#include <gtest/gtest.h>
#include <tuple>
#include <vector>

namespace Test::ZipDirFs::Zip
{
	struct ArchiveAccess : public ::ZipDirFs::Zip::Archive
	{
		static ::ZipDirFs::Zip::Base::Lib* const& getData(::ZipDirFs::Zip::Archive&);
		static std::vector<std::string>& getNames(::ZipDirFs::Zip::Archive&);
		static std::map<std::string, std::tuple<std::uint64_t, bool>>& getNameAttributes(
			::ZipDirFs::Zip::Archive&);
		static std::map<std::uint64_t, std::weak_ptr<::ZipDirFs::Zip::Entry>>& getEntries(
			::ZipDirFs::Zip::Archive&);
		static void invokePopulate(::ZipDirFs::Zip::Archive&);
		static void registerArchive(const std::shared_ptr<Archive>&);
		static void cleanupArchive();

	protected:
		struct FactoryAccess : public ::ZipDirFs::Zip::Factory
		{
			static std::map<const ::ZipDirFs::Zip::Base::Lib*,
				std::weak_ptr<::ZipDirFs::Zip::Archive>>&
				getArchivesByData();
		};
	};

	struct ArchiveIteratorAccess : public ::ZipDirFs::Zip::Archive::ArchiveIterator
	{
		static ::ZipDirFs::Zip::Archive::ArchiveIterator create(
			const std::shared_ptr<::ZipDirFs::Zip::Archive>&, const std::shared_ptr<std::string>&,
			std::vector<std::string>::iterator&&);
		static std::shared_ptr<::ZipDirFs::Zip::Archive>& getArchive(
			::ZipDirFs::Zip::Archive::ArchiveIterator&);
		static std::shared_ptr<std::string>& getPrefix(::ZipDirFs::Zip::Archive::ArchiveIterator&);
		static std::vector<std::string>::iterator& getCurrent(
			::ZipDirFs::Zip::Archive::ArchiveIterator&);
		static std::string& getFilename(::ZipDirFs::Zip::Archive::ArchiveIterator&);
		static ::ZipDirFs::Zip::Archive::ArchiveIterator make(
			const std::shared_ptr<::ZipDirFs::Zip::Archive>&);
		static ::ZipDirFs::Zip::Archive::ArchiveIterator make(
			const std::shared_ptr<::ZipDirFs::Zip::Archive>&, std::vector<std::string>::iterator&&);
		static ::ZipDirFs::Zip::Archive::ArchiveIterator make(
			const std::shared_ptr<::ZipDirFs::Zip::Archive>&, const std::string&);

	protected:
		ArchiveIteratorAccess(const std::shared_ptr<::ZipDirFs::Zip::Archive>&,
			const std::shared_ptr<std::string>&, std::vector<std::string>::iterator);
		ArchiveIteratorAccess(const std::shared_ptr<::ZipDirFs::Zip::Archive>&);
		ArchiveIteratorAccess(
			const std::shared_ptr<::ZipDirFs::Zip::Archive>&, std::vector<std::string>::iterator&&);
		ArchiveIteratorAccess(const std::shared_ptr<::ZipDirFs::Zip::Archive>&, const std::string&);
	};

	struct ArchiveTest : public ::testing::Test
	{
		void TearDown();

	protected:
		std::shared_ptr<::ZipDirFs::Zip::Archive> CreateArchive(Fixtures::Lib&);
		std::tuple<std::string, std::uint64_t> MakeEntry(
			const std::shared_ptr<::ZipDirFs::Zip::Archive>&, const std::string&);
		std::tuple<std::string, std::uint64_t, std::shared_ptr<::ZipDirFs::Zip::Entry>> CreateEntry(
			const std::shared_ptr<::ZipDirFs::Zip::Archive>&, const std::string&);
		void MockNameSequence(Fixtures::Lib&, ::ZipDirFs::Zip::Archive&, std::vector<std::string>);
	};

	struct ArchiveIteratorTest : public ::testing::Test
	{
	protected:
		std::shared_ptr<::ZipDirFs::Zip::Archive> CreateArchive(Fixtures::Lib&, int times = 1);
		void InitializeNames(::ZipDirFs::Zip::Archive&, std::vector<std::string>&&);
	};
} // namespace Test::ZipDirFs::Zip

#endif // TEST_ZIPDIRFS_ZIP_ARCHIVE_H
