/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "FuseDaemon.h"

namespace Test::ZipDirFs::Fuse
{
	namespace Fixtures
	{
		FuseDaemon::FuseDaemon(const std::string& mountPoint, const std::string& fsName,
			std::unique_ptr<::fusekit::entry>&& root) :
			mountPoint(mountPoint),
			daemon(initDaemon(std::move(root))),
			options("ro,nosuid,noexec,noatime,subtype=zipdirfs_test,fsname=" + fsName),
			argv(initArguments(mountPoint))
		{
		}
		FuseDaemon::~FuseDaemon()
		{
			delete[] argv;
			daemon.root() = nullptr;
		}
		void FuseDaemon::stop() { (void)!::system(std::string("fusermount -u " + mountPoint).c_str()); }
		::fusekit::daemon<EntryProxy>& FuseDaemon::initDaemon(
			std::unique_ptr<::fusekit::entry>&& root)
		{
			::fusekit::daemon<EntryProxy>& daemon = ::fusekit::daemon<EntryProxy>::instance();
			daemon.root() = std::move(root);
			return daemon;
		}
		const char** FuseDaemon::initArguments(const std::string& mountPoint)
		{
			return new const char* [5] { "gtest", "-d", "-o", options.c_str(), mountPoint.c_str() };
		}
		void FuseDaemon::run() { daemon.run(5, const_cast<char**>(argv)); }
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Fuse
