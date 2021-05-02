/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_FUSE_FIXTURES_FUSEDAEMONTHREAD_H
#define TEST_ZIPDIRFS_FUSE_FIXTURES_FUSEDAEMONTHREAD_H

#include "FuseDaemon.h"
#include <functional>

namespace Test::ZipDirFs::Fuse
{
	namespace Fixtures
	{
		struct FuseDaemonThread : public FuseDaemon
		{
			FuseDaemonThread(const std::string& mountPoint, const std::string& fsName,
				std::unique_ptr<::fusekit::entry>&& root, std::function<void()> worker);
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Fuse

#endif // TEST_ZIPDIRFS_FUSE_FIXTURES_FUSEDAEMONTHREAD_H
