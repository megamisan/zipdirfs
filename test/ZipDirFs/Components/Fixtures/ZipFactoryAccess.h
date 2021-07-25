/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_COMPONENTS_FIXTURES_ZIPFACTORYACCESS_H
#define TEST_ZIPDIRFS_COMPONENTS_FIXTURES_ZIPFACTORYACCESS_H

#include "ZipDirFs/Zip/Factory.h"

namespace Test::ZipDirFs::Components
{
	namespace Fixtures
	{
		struct ZipFactoryAccess : public ::ZipDirFs::Zip::Factory
		{
			static std::map<boost::filesystem::path, std::weak_ptr<::ZipDirFs::Zip::Archive>>&
				getArchivesByPath();
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Components

#endif // TEST_ZIPDIRFS_COMPONENTS_FIXTURES_ZIPFACTORYACCESS_H
