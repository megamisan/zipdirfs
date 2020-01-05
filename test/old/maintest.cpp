/*
 * Copyright © 2012 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include <string>
//#include <iostream>
//#include <cstdio>
//#include <cstddef>
//#include <unistd.h>
//#include <zip.h>
//#include <fcntl.h>

#ifdef TEST
#if TEST == 1
int testTree(std::string &path);
#endif
#if TEST == 2
int testTreeIterator(std::string &path);
#endif
#if TEST == 3
int testDirectoryFactory(std::string &path);
#endif
#if TEST == 4
int testZipOpen(std::string &path);
#endif
#if TEST == 5
int testZipEnumeration(std::string &path);
#endif
#if TEST == 6
int testZipIterator(std::string &path);
#endif
#if TEST == 7
int testZipWalker(std::string &path);
#endif
#if TEST == 8
int testTreeRemove(std::string &path);
#endif
#endif

int main(int argc, char *argv[])
{
	std::string path(".");
	if (argc > 1)
	{
		path = argv[1];
	}
#ifdef TEST
#if TEST == 1
	return testTree(path);
#endif
#if TEST == 2
	return testTreeIterator(path);
#endif
#if TEST == 3
	return testDirectoryFactory(path);
#endif
#if TEST == 4
	return testZipOpen(path);
#endif
#if TEST == 5
	return testZipEnumeration(path);
#endif
#if TEST == 6
	return testZipIterator(path);
#endif
#if TEST == 7
	return testZipWalker(path);
#endif
#if TEST == 8
	return testTreeRemove(path);
#endif
#endif
}
