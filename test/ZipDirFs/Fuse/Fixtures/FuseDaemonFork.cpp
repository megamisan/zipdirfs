/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "FuseDaemonFork.h"
#include <boost/filesystem.hpp>
#include <functional>
#include <string>
#include <thread>
#include <vector>
#include <wait.h>

namespace Test::ZipDirFs::Fuse
{
	namespace Fixtures
	{
		int startDaemonProcess(
			const std::string&, std::vector<int>, std::vector<int>, std::vector<std::string>);
		void renumber(std::vector<int>);

		FuseDaemonFork::FuseDaemonFork(const std::string& mountPoint, const std::string& fsName,
			std::unique_ptr<::fusekit::entry>&& root, std::function<void(std::vector<int>&)> worker,
			const std::string& helperName, const int (&fdCount)[2],
			std::vector<std::string> arguments) :
			FuseDaemon(mountPoint, fsName, std::move(root))
		{
			std::vector<int> parentPipes, childPipes;
			for (int step = 0; step < 2; ++step)
			{
				for (int index = 0; index < fdCount[step]; ++index)
				{
					int fds[2];
					if (pipe(fds) < 0)
					{
						throw std::runtime_error("Cannot allocate pipes");
					}
					parentPipes.push_back(fds[1 - step]);
					childPipes.push_back(fds[step]);
				}
			}
			int pid = startDaemonProcess(helperName, parentPipes, childPipes, arguments);
			if (pid != -1)
			{
				std::thread t(worker, std::ref(parentPipes));
				run();
				waitpid(pid, NULL, 0);
				t.join();
			}
		}
		int startDaemonProcess(const std::string& helperName, std::vector<int> parentPipes,
			std::vector<int> childPipes, std::vector<std::string> arguments)
		{
			std::vector<const char*> argv;
			std::string exe =
				(::boost::filesystem::read_symlink("/proc/self/exe").parent_path() / "gtest_helper")
					.native();
			arguments.insert(arguments.begin(), helperName);
			for (auto it = arguments.begin(), endIt = arguments.end(); it != endIt; ++it)
			{
				argv.push_back(it->c_str());
			}
			argv.push_back(nullptr);
			int pid = fork();
			if (pid == -1 || pid == 0)
			{
				for (auto it = parentPipes.begin(); it != parentPipes.end(); ++it)
				{
					close(*it);
				}
			}
			if (pid == 0)
			{
				close(STDIN_FILENO);
				renumber(childPipes);
				usleep(10000);
				execv(exe.c_str(), const_cast<char* const*>(argv.data()));
				perror("Fork");
				exit(-1);
			}
			for (auto it = childPipes.begin(); it != childPipes.end(); ++it)
			{
				close(*it);
			}
			return pid;
		}
		void renumber(std::vector<int> fds)
		{
			int next = 3, newFd, oldFd;
			for (auto it = fds.begin(), endId = fds.end(); it != endId; ++it)
			{
				oldFd = *it;
				newFd = next++;
				if (oldFd == newFd)
				{
					continue;
				}
				if (dup2(oldFd, newFd) == newFd)
				{
					close(oldFd);
					*it = newFd;
				}
			}
		}
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Fuse
