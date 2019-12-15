/*
 * Copyright © 2012 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 *
 * This file is part of zipdirfs.
 *
 * zipdirfs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * zipdirfs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with zipdirfs.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id$
 */
#include "ZipDirFs/NameSearchTree.h"
#include "ZipDirFs/SystemDirectoryFactory.h"
#include "ZipDirFs/ZipFile.h"
#include "ZipDirFs/ZipIterator.h"
#include "ZipDirFs/ZipWalker.h"
#include <string>
#include <iostream>
#include <dirent.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <zip.h>
#include <fcntl.h>

int testTree(std::string &path);
int testTreeIterator(std::string &path);
int testDirectoryFactory(std::string &path);
int testZipOpen(std::string &path);
int testZipEnumeration(std::string &path);
int testZipIterator(std::string &path);
int testZipWalker(std::string &path);
int testTreeRemove(std::string &path);

int main(int argc, char *argv[])
{
	std::string path(".");
	if (argc > 1)
	{
		path = argv[1];
	}
	// return testTree(path);
	// return testTreeIterator(path);
	// return testDirectoryFactory(path);
	// return testZipOpen(path);
	// return testZipEnumeration(path);
	// return testZipIterator(path);
	// return testZipWalker(path);
	return testTreeRemove(path);
}

int buildTree(std::string& path, ZipDirFs::NameSearchTree<int>& tree)
{
	DIR *folder = NULL;
	folder = opendir(path.c_str());
	if (folder == NULL)
	{
		perror("opendir");
		return -1;
	}
	dirent *entry = NULL;
	int counter = 0;
	while ((entry = readdir(folder))/* && (counter < 200)*/)
	{
		tree.add(entry->d_name, ++counter);
	}
	closedir(folder);
	return 0;
}

int testTree(std::string& path)
{
	ZipDirFs::NameSearchTree<int> tree;
	int res = buildTree(path, tree);
	std::cout << tree;
	return res;
}

int testTreeIterator(std::string& path)
{
	ZipDirFs::NameSearchTree<int> tree;
	int res = buildTree(path, tree);
	for (ZipDirFs::NameSearchTree<int>::iterator it(tree.begin()); it != tree.end(); it++)
	{
		std::cout << *it << std::endl;
	}
	return res;
}

int fusefiller(void*, const char* name, const struct stat*, off_t)
{
	std::cout << name << std::endl;
	return 0;
}

int testDirectoryFactory(std::string &path)
{
	ZipDirFs::SystemDirectoryFactory<> entry;
	entry.setRealPath(path.c_str());
	std::cout << "Real path " << entry.getRealPath() << std::endl;
	fuse_file_info dummy;
	entry.readdir(NULL, &fusefiller, 0, dummy);
	return 0;
}

int testZipOpen(std::string &path)
{
	int handle = open(path.c_str(), O_RDONLY | O_NOATIME | O_NOCTTY);
	if (handle < 0)
	{
		perror("open");
		return -1;
	}
	int error;
	zip* zipFile = zip_fdopen(handle, 0, &error);
	if (zipFile == NULL)
	{
		int len = zip_error_to_str(NULL, 0, error, errno);
		char *message = new char[len + 1];
		zip_error_to_str(message, 1024, error, errno);
		std::cerr << "zip_fdopen: " << message << std::endl;
		delete[] message;
		close(handle);
		return -1;
	}
	std::cout << path << " is a zip file." << std::endl;
	zip_close(zipFile);
	return 0;
}

int testZipEnumeration(std::string &path)
{
	int handle = open(path.c_str(), O_RDONLY | O_NOATIME | O_NOCTTY);
	if (handle < 0)
	{
		perror("open");
		return -1;
	}
	int error;
	zip* zipFile = zip_fdopen(handle, 0, &error);
	if (zipFile == NULL)
	{
		int len = zip_error_to_str(NULL, 0, error, errno);
		char *message = new char[len + 1];
		zip_error_to_str(message, 1024, error, errno);
		std::cerr << "zip_fdopen: " << message << std::endl;
		delete[] message;
		close(handle);
		return -1;
	}
	zip_int64_t numEntries = zip_get_num_entries(zipFile, 0);
	struct zip_stat fileinfo;
	for (zip_int64_t i = 0; i < numEntries; i++)
	{
		if (zip_stat_index(zipFile, i, 0, &fileinfo) != 0)
		{
			std::cerr << "Error at index " << i << std::endl;
			continue;
		}
		if (fileinfo.valid & (ZIP_STAT_NAME | ZIP_STAT_SIZE | ZIP_STAT_MTIME))
		{
			std::cout << i << " " << fileinfo.name << " (" << fileinfo.size << ") @" << fileinfo.mtime << std::endl;
		}
		else
		{
			std::cerr << "Not all required information for entry " << i << std::endl;
		}
	}
	zip_close(zipFile);
	return 0;
}

int testZipIterator(std::string &path)
{
	ZipDirFs::ZipFile file(path);
	ZipDirFs::ZipIterator end = file.end();
	for (ZipDirFs::ZipIterator it(file.begin()); it != end; it++)
	{
		std::cout << it->name << std::endl;
	}
	return 0;
}

int testZipWalker(std::string &path)
{
	ZipDirFs::ZipFile file(path);
	ZipDirFs::ZipWalker end(&file, "", true);
	for (ZipDirFs::ZipWalker it(&file, "", false); it != end; it++)
	{
		std::cout << it->first << std::endl;
		delete it->second;
	}
	return 0;
}

int testTreeRemove(std::string &path)
{
	ZipDirFs::NameSearchTree<int> tree;
	int value = 0;
	tree.add("name", value);
	std::cout << "Added name" << std::endl << tree;
	value++;
	tree.add("name1", value);
	std::cout << "Added name1" << std::endl << tree;
	tree.remove("name1");
	std::cout << "Removed name1" << std::endl << tree;
	return 0;
}
