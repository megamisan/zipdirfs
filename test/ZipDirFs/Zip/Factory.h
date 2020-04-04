/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_ZIP_FACTORY_H
#define TEST_ZIPDIRFS_ZIP_FACTORY_H

#include "ZipDirFs/Zip/Factory.h"
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

namespace Test::ZipDirFs::Zip
{
	struct FactoryAccess : public ::ZipDirFs::Zip::Factory
	{
		static ::ZipDirFs::Zip::Factory*& getInstance();
		static std::map<boost::filesystem::path, std::weak_ptr<::ZipDirFs::Zip::Archive>>&
			getArchivesByPath();
		static std::map<const ::ZipDirFs::Zip::Base::Lib*, std::weak_ptr<::ZipDirFs::Zip::Archive>>&
			getArchivesByData();
	};

	struct FactoryOverride
	{
		FactoryOverride(::ZipDirFs::Zip::Factory* temporary);
		~FactoryOverride();

	protected:
		::ZipDirFs::Zip::Factory* const original;
	};

	struct FactoryTest : public ::testing::Test
	{
		void TearDown();
		boost::filesystem::path generatePath();
	};

} // namespace Test::ZipDirFs::Zip

#endif // TEST_ZIPDIRFS_ZIP_FACTORY_H
