/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_COMPONENTS_FIXTURES_LIBINSTANCE_H
#define TEST_ZIPDIRFS_COMPONENTS_FIXTURES_LIBINSTANCE_H

#include "ZipDirFs/Zip/Base/Lib.h"

namespace Test::ZipDirFs::Components
{
	namespace Fixtures
	{
		struct LibInstance : public ::ZipDirFs::Zip::Base::Lib
		{
			LibInstance() : ::ZipDirFs::Zip::Base::Lib() {}
			~LibInstance() {}
		};

	} // namespace Fixtures
} // namespace Test::ZipDirFs::Components

#endif // TEST_ZIPDIRFS_COMPONENTS_FIXTURES_LIBINSTANCE_H
