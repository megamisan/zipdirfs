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
#include "NameSearchTree.h"
#include "SystemDirectoryFactory.h"
#include <string>
#include <iostream>
#include <dirent.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

int testTree(std::string &path);
int testTreeIterator(std::string &path);
int testDirectoryFactory(std::string &path);

int main(int argc, char *argv[])
{
	std::string path(".");
	if (argc > 1)
	{
		path = argv[1];
	}
	// return testTree(path);
	// return testTreeIterator(path);
	return testDirectoryFactory(path);
}

int buildTree(std::string& path, NameSearchTree<int>& tree)
{
	DIR *folder = NULL;
	folder = opendir(path.c_str());
	if (folder == NULL)
	{
		perror("opendir");
		return -1;
	}
	int bufferLength = offsetof(dirent, d_name) + pathconf(path.c_str(), _PC_NAME_MAX) + 1;
	char *buffer = new char[bufferLength];
	dirent *entry = reinterpret_cast<dirent*>(buffer);
	dirent *result = NULL;
	int res = readdir_r(folder, entry, &result);
	int counter = 0;
	while ((!res) && (result)/* && (counter < 200)*/)
	{
		tree.add(entry->d_name, ++counter);
		res = readdir_r(folder, entry, &result);
	}
	if (res)
	{
		perror("readdir_r");
		closedir(folder);
		return -2;
	}
	closedir(folder);
	delete[] buffer;
	return 0;
}

int testTree(std::string& path)
{
	NameSearchTree<int> tree;
	int res = buildTree(path, tree);
	std::cout << tree;
	return res;
}

int testTreeIterator(std::string& path)
{
	NameSearchTree<int> tree;
	int res = buildTree(path, tree);
	for (NameSearchTree<int>::iterator it(tree.begin()); it != tree.end(); it++)
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
	SystemDirectoryFactory<> entry;
	entry.setRealPath(path.c_str());
	std::cout << "Real path " << entry.getRealPath() << std::endl;
	fuse_file_info dummy;
	entry.readdir(NULL, &fusefiller, 0, dummy);
	return 0;
}
