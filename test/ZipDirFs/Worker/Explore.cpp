/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "test/gtest_helper.h"
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <tuple>
#include <unistd.h>
#include <vector>

int explore(int, char**);

TEST_HELPER(Explore, &explore);

#define HANDLE_ERROR(expression) \
	if ((expression) < 0) \
	{ \
		perror(("Worker L" + std::to_string(__LINE__)).c_str()); \
		return -1; \
	}

int explore(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Helper: Missing path" << std::endl;
		return -1;
	}
	std::vector<std::tuple<std::string, std::string>> remainingPaths({{argv[1], std::string("")}});
	while (remainingPaths.size())
	{
		auto currentEntry = remainingPaths.front();
		remainingPaths.erase(remainingPaths.begin());
		auto& currentPath = std::get<0>(currentEntry);
		auto& innerPath = std::get<1>(currentEntry);
		struct stat stbuf;
		short len = innerPath.length();
		HANDLE_ERROR(lstat(currentPath.c_str(), &stbuf))
		HANDLE_ERROR(write(3, &stbuf.st_mode, sizeof(stbuf.st_mode)));
		HANDLE_ERROR(write(3, &stbuf.st_ctim.tv_sec, sizeof(stbuf.st_ctim.tv_sec)));
		HANDLE_ERROR(write(3, &stbuf.st_atim.tv_sec, sizeof(stbuf.st_atim.tv_sec)));
		HANDLE_ERROR(write(3, &stbuf.st_mtim.tv_sec, sizeof(stbuf.st_mtim.tv_sec)));
		HANDLE_ERROR(write(3, &stbuf.st_nlink, sizeof(stbuf.st_nlink)));
		HANDLE_ERROR(write(3, &stbuf.st_size, sizeof(stbuf.st_size)));
		HANDLE_ERROR(write(3, &len, sizeof(len)));
		HANDLE_ERROR(write(3, innerPath.c_str(), innerPath.length()));
		std::cerr << "Wrote header: " << 46 + innerPath.length() << std::endl;
		if (S_ISDIR(stbuf.st_mode))
		{
			auto dirHandle = opendir(currentPath.c_str());
			if (dirHandle != nullptr)
			{
				struct dirent* dirbuf = nullptr;
				errno = 0;
				while ((dirbuf = readdir(dirHandle)) != nullptr)
				{
					if ((dirbuf->d_name[0] == '.')
						&& ((dirbuf->d_name[1] == 0)
							|| ((dirbuf->d_name[1] == '.') && (dirbuf->d_name[2] == 0))))
					{
						continue;
					}
					remainingPaths.emplace_back(
						currentPath + "/" + dirbuf->d_name, innerPath + "/" + dirbuf->d_name);
					errno = 0;
				}
				if (errno != 0)
				{
					perror("Worker");
					return -1;
				}
				closedir(dirHandle);
			};
		}
		if (S_ISREG(stbuf.st_mode))
		{
			char buf[4096];
			auto file = open(currentPath.c_str(), O_RDONLY);
			HANDLE_ERROR(file);
			int len;
			while ((len = read(file, buf, sizeof(buf))) > 0)
			{
				HANDLE_ERROR(write(3, buf, len));
				std::cerr << "Wrote buffer part: " << len << " of " << stbuf.st_size << std::endl;
			}
			HANDLE_ERROR(len)
			close(file);
		}
		if (S_ISLNK(stbuf.st_mode))
		{
			char* linkbuf = new char[stbuf.st_size];
			int len = readlink(currentPath.c_str(), linkbuf, stbuf.st_size);
			HANDLE_ERROR(len);
			if (len >= 0)
			{
				HANDLE_ERROR(write(3, linkbuf, len));
				std::cerr << "Wrote target: " << len << std::endl;
			}
			delete[] linkbuf;
		}
	}
	return 0;
}
