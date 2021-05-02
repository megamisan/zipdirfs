/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_FUSE_FIXTURES_ONECHANGED_H
#define TEST_ZIPDIRFS_FUSE_FIXTURES_ONECHANGED_H

#include "ZipDirFs/Containers/Helpers/Changed.h"

namespace Test::ZipDirFs::Fuse
{
	namespace Fixtures
	{
		struct OneChanged : public ::ZipDirFs::Containers::Helpers::Changed
		{
			OneChanged(std::time_t);
			bool operator()();
			operator std::time_t() const;
		private:
			bool called;
			std::time_t time;
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Fuse

#endif // TEST_ZIPDIRFS_FUSE_FIXTURES_ONECHANGED_H
