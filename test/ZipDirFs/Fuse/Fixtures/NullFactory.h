/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_FUSE_FIXTURES_NULLFACTORY_H
#define TEST_ZIPDIRFS_FUSE_FIXTURES_NULLFACTORY_H

#include "ZipDirFs/Containers/Helpers/Factory.h"

namespace Test::ZipDirFs::Fuse
{
	namespace Fixtures
	{
		struct NullFactory : public ::ZipDirFs::Containers::Helpers::Factory
		{
			const mapped_type create(const key_type&);
			void destroy(const mapped_type&);
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Fuse

#endif // TEST_ZIPDIRFS_FUSE_FIXTURES_NULLFACTORY_H
