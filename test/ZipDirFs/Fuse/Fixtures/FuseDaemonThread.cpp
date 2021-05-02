/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "FuseDaemonThread.h"
#include <thread>

namespace Test::ZipDirFs::Fuse
{
	namespace Fixtures
	{
		FuseDaemonThread::FuseDaemonThread(const std::string& mountPoint, const std::string& fsName,
			std::unique_ptr<::fusekit::entry>&& root, std::function<void()> worker) :
			FuseDaemon(mountPoint, fsName, std::move(root))
		{
			std::thread t(worker);
			run();
			t.join();
		}
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Fuse
