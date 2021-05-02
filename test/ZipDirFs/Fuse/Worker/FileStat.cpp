/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "test/gtest_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

int fileStat(int, char**);

TEST_HELPER(FileStat, &fileStat);

int fileStat(int argc, char** argv) {
	if (argc != 2)
	{
		std::cerr << "Helper: Missing path" << std::endl;
		return -1;
	}
	struct stat stbuf;
	int result = lstat(argv[1], &stbuf);
	if (write(3, &result, sizeof(result)) == -1) {
		perror("Helper");
		return -1;
	}
	if (write(3, &stbuf, sizeof(stbuf)) == -1) {
		perror("Helper");
		return -1;
	}
	return 0;
}
