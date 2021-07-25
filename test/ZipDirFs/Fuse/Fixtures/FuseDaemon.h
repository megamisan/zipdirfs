/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_FUSE_FIXTURES_FUSEDAEMON_H
#define TEST_ZIPDIRFS_FUSE_FIXTURES_FUSEDAEMON_H

#include "ZipDirFs/Fuse/EntryProxy.h"
#include <fusekit/daemon.h>
#include <memory>

namespace Test::ZipDirFs::Fuse
{
	namespace Fixtures
	{
		using ::ZipDirFs::Fuse::EntryProxy;

		struct FuseDaemon
		{
			FuseDaemon(const std::string& mountPoint, const std::string& fsName,
				std::unique_ptr<::fusekit::entry>&& root);
			~FuseDaemon();
			void stop();

		protected:
			::fusekit::daemon<EntryProxy>& initDaemon(std::unique_ptr<::fusekit::entry>&& root);
			const char** initArguments(const std::string& mountPoint);
			void run();
			const std::string mountPoint;
			::fusekit::daemon<EntryProxy>& daemon;
			const std::string options;
			const char** const argv;
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Fuse

#endif // TEST_ZIPDIRFS_FUSE_FIXTURES_FUSEDAEMON_H
