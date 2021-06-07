/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_ZIP_STREAM_H
#define TEST_ZIPDIRFS_ZIP_STREAM_H

#include "Fixtures/Lib.h"
#include "ZipDirFs/Zip/Archive.h"
#include "ZipDirFs/Zip/Entry.h"
#include "ZipDirFs/Zip/Factory.h"
#include "ZipDirFs/Zip/Stream.h"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Zip
{
	using LibBase = ::ZipDirFs::Zip::Base::Lib;

	struct StreamAccess : public ::ZipDirFs::Zip::Stream
	{
		static ::ZipDirFs::Zip::Base::Content& getStreamBufferContent(::ZipDirFs::Zip::Stream&);
		static std::basic_streambuf<::ZipDirFs::Zip::Stream::char_type>*& getStreamStreamBuffer(
			::ZipDirFs::Zip::Stream&);
		struct GlobalHelper
		{
			GlobalHelper(std::uint64_t);
			~GlobalHelper();
			::ZipDirFs::Zip::Archive& getArchive();
			::ZipDirFs::Zip::Entry& getEntry();
			const boost::filesystem::path& getPath();
			const std::string& getName();
			Fixtures::Lib& getLib();
			::ZipDirFs::Zip::Base::Content& getEntryContent();

		protected:
			LibBase* const data;
			Fixtures::Lib lib;
			const boost::filesystem::path path;
			const std::string name;
			::ZipDirFs::Zip::Archive archiveInstance;
			const std::shared_ptr<::ZipDirFs::Zip::Archive> archive;
			::ZipDirFs::Zip::Entry entryInstance;
			const std::shared_ptr<::ZipDirFs::Zip::Entry> entry;
		};

	protected:
		struct StreamBufferAccess : public std::basic_streambuf<::ZipDirFs::Zip::Stream::char_type>
		{
			static ::ZipDirFs::Zip::Base::Content& getContent(
				std::basic_streambuf<::ZipDirFs::Zip::Stream::char_type>*);

		protected:
			const std::shared_ptr<::ZipDirFs::Zip::Base::Content> content;
		};
		struct EntryAccess : public ::ZipDirFs::Zip::Entry
		{
			static ::ZipDirFs::Zip::Base::Content& getContent(::ZipDirFs::Zip::Entry&);
		};
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
			static std::map<boost::filesystem::path, std::weak_ptr<::ZipDirFs::Zip::Archive>>&
				getArchivesByPath();
		};
	};

	struct StreamTest : public ::testing::Test
	{
	};

	struct StreamBufferTest : public ::testing::Test
	{
		void SetUp();
		void TearDown();

	protected:
		std::shared_ptr<::ZipDirFs::Zip::Base::Content> content;
		std::shared_ptr<std::basic_streambuf<::ZipDirFs::Zip::Stream::char_type>> buffer;
		void GenerateRandomCompletedContentState();
		std::unique_ptr<::ZipDirFs::Zip::Stream::char_type[]> GenerateRandomBufferedContentState();
		void GenerateRandomSegment(::ZipDirFs::Zip::Stream::char_type*, std::streamsize);
		static constexpr std::uint64_t randomLenModulo();
		static constexpr std::uint64_t randomLenBase();
		static constexpr std::uint64_t randomLenMax();
	};
} // namespace Test::ZipDirFs::Zip

#endif // TEST_ZIPDIRFS_ZIP_STREAM_H
