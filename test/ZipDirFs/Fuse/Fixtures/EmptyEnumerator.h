/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_FUSE_FIXTURES_EMPTYENUMERATOR_H
#define TEST_ZIPDIRFS_FUSE_FIXTURES_EMPTYENUMERATOR_H

#include "ZipDirFs/Containers/Helpers/Enumerator.h"
#include <string>

namespace Test::ZipDirFs::Fuse
{
	namespace Fixtures
	{
		struct EmptyEnumerator : public ::ZipDirFs::Containers::Helpers::Enumerator<std::string>
		{
			void reset();
			void next();
			bool valid();
			const std::string& current();
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Fuse

#endif // TEST_ZIPDIRFS_FUSE_FIXTURES_EMPTYENUMERATOR_H
