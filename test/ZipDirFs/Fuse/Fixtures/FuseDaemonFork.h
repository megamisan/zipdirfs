/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_FUSE_FIXTURES_FUSEDAEMONFORK_H
#define TEST_ZIPDIRFS_FUSE_FIXTURES_FUSEDAEMONFORK_H

#include "FuseDaemon.h"
#include <functional>
#include <vector>

namespace Test::ZipDirFs::Fuse
{
	namespace Fixtures
	{
		struct FuseDaemonFork : public FuseDaemon
		{
			FuseDaemonFork(const std::string& mountPoint, const std::string& fsName,
				std::unique_ptr<::fusekit::entry>&& root,
				std::function<void(std::vector<int>&)> worker, const std::string& helperName,
				const int (&fdCount)[2], std::vector<std::string> arguments);
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Fuse

#endif // TEST_ZIPDIRFS_FUSE_FIXTURES_FUSEDAEMONFORK_H
