/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipFactoryAccess.h"
#include <boost/filesystem.hpp>

namespace Test::ZipDirFs::Components
{
	namespace Fixtures
	{
		std::map<boost::filesystem::path, std::weak_ptr<::ZipDirFs::Zip::Archive>>&
			ZipFactoryAccess::getArchivesByPath()
		{
			return Factory::archivesByPath;
		}
		std::map<const ::ZipDirFs::Zip::Base::Lib*, std::weak_ptr<::ZipDirFs::Zip::Archive>>&
			ZipFactoryAccess::getArchivesByData()
		{
			return Factory::archivesByData;
		}

	} // namespace Fixtures
} // namespace Test::ZipDirFs::Components
