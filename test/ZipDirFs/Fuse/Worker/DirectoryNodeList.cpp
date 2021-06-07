/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "test/gtest_helper.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <vector>

int directoryNodeList(int, char**);

TEST_HELPER(DirectoryNodeList, &directoryNodeList);

int directoryNodeList(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Helper: Missing path" << std::endl;
		return -1;
	}
	std::string folderPath(argv[1]);
	std::vector<std::string> result;
	::boost::filesystem::directory_iterator end;
	::boost::filesystem::directory_iterator current(folderPath);
	while (!(current == end))
	{
		result.push_back(current->path().leaf().native());
		++current;
	}
	for (auto it = result.begin(), endIt = result.end(); it != endIt; ++it)
	{
		if (write(3, it->c_str(), it->length() + 1) == -1)
		{
			perror("Helper");
			return -1;
		}
	}
	return 0;
}
