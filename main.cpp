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
#include "zipdirfs/entry_definitions.h"
#include <fusekit/daemon.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string.h>

typedef fusekit::daemon<zipdirfs::system_directory> daemon_type;

const char *getProgramName(const char* self);
bool parseParameters(std::vector<const char*> &args, std::string &path);
void showUsage(const char *self);
bool isHelp(const char* s1) { return !strcmp(s1, "-h"); }

int main(int argc, const char *argv[])
{
	const char* const programName = getProgramName(argv[0]);
	// for (int i = 0; i < argc; i++) std::cerr << "args: " << argv[i] << std::endl;
	std::string sourcePath;
	std::vector<const char*> arguments(argv, argv + argc);
	if (std::find_if(arguments.begin(), arguments.end(), isHelp) != arguments.end())
	{
		showUsage(argv[0]);
		return 0;
	}
	if (!parseParameters(arguments, sourcePath))
	{
		const char *message = (sourcePath.empty()) ? ": missing originalpath argument" : ": missing mountpoint argument";
		std::cerr << programName << message << std::endl;
		return -1;
	}
	if (sourcePath[sourcePath.size() - 1] == '/') sourcePath.erase(sourcePath.size() - 1);
	char *fstype = new char[strlen(programName) + 9];
	fstype[0] = 0;
	strcat(fstype, "subtype=");
	strcat(fstype, programName);
	char *fsname = new char[sourcePath.size() + 8];
	fsname[0] = 0;
	strcat(fsname, "fsname=");
	strcat(fsname, sourcePath.c_str());
	arguments.push_back("-oro,nosuid,noexec,noatime");
	arguments.push_back("-o");
	arguments.push_back(fstype);
	arguments.push_back("-o");
	arguments.push_back(fsname);
	daemon_type &daemon = daemon_type::instance();
	daemon.root().setRealPath(sourcePath.c_str());
	// for (std::vector<const char*>::iterator it = arguments.begin(); it != arguments.end(); it++) std::cerr << "fuse args:" << *it << std::endl;
	daemon.run(arguments.size(), const_cast<char **>(&arguments[0]));
	delete[] fsname;
	delete[] fstype;
	return 0;
}

const char *getProgramName(const char* self)
{
	const char *program = strrchr(self, '/');
	return (program == NULL) ? self : (program + 1);
}

void showUsage(const char* self)
{
	std::cerr << "real usage: " << self << " originalpath mountpoint [options]" << std::endl;
	const char * argv[2] = { self, "-h" };
	daemon_type::instance().run(2, const_cast<char **>(argv));
	std::cerr << std::endl
		<< "zipdirfs options:" << std::endl
		<< "    originalpath           the path to mount from and filter for zip files" << std::endl;
}

bool parseParameters(std::vector<const char*> &args, std::string &path)
{
	bool hasMountPoint = false;
	bool hasMountSource = false;
	std::vector<const char*>::iterator itPath;
	std::vector<const char*>::iterator it = args.begin();
	for (it++; it != args.end(); it++) // Skip first parameter: program path and name
	{
		if (*it[0] == '-')
		{
			continue;
		}
		if (!hasMountPoint)
		{
			path = *it;
			hasMountPoint = true;
			itPath = it;
		}
		else if (!hasMountSource)
		{
			hasMountSource = true;
		}
	}
	if (hasMountSource)
	{
		args.erase(itPath);
	}
	return hasMountSource;
}
