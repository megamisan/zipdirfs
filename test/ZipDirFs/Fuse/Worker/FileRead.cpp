/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "test/gtest_helper.h"
#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

int fileRead(int, char**);

TEST_HELPER(FileRead, &fileRead);

int fileRead(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Helper: Missing path" << std::endl;
		return -1;
	}
	int file = open(argv[1], O_RDONLY);
	if (file == -1)
	{
		perror("Helper");
		return -1;
	}
	std::streamsize length = 0;
	char buffer[4096];
	while ((length = read(file, buffer, sizeof(buffer))) > 0)
	{
		if ((write(3, &length, sizeof(length))) == -1)
		{
			perror("Helper");
			return -1;
		}
		if ((write(3, buffer, length)) == -1)
		{
			perror("Helper");
			return -1;
		}
	}
	if (length < 0)
	{
		perror("Helper");
		return -1;
	}
	else
	{
		if ((write(3, &length, sizeof(length))) == -1)
		{
			perror("Helper");
			return -1;
		}
	}
	close(file);
	return 0;
}
