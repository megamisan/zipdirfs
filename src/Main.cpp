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
#include "Main.h"
#include "zipdirfs/entry_definitions.h"
#include "CommandLine.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <fusekit/daemon.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string.h>

typedef fusekit::daemon<zipdirfs::system_directory> daemon_type;

namespace po = boost::program_options;
Main application;

const char *getProgramName(const char* self);
bool parseParameters(std::vector<const char*> &args, std::string &path);
void showUsage(const char *self);
void showVersion(const char *self);
bool isHelp(const char* s1) { return !strcmp(s1, "-h") || !strcmp(s1, "--help"); }
bool isVersion(const char* s1) { return !strcmp(s1, "-V") || !strcmp(s1, "--version"); }
char** toArgv(std::vector<std::string>& arguments);
void freeArgv(char** argv);

int main(const int argc, const char** argv)
{
	try
	{
		app.Init(argc, argv);
		app.Run();
	}
	catch (Main::Result res)
	{
		return res.result;
	}
	return 0;
}

void Main::Run() throw(Main::Result)
{
	this->fuseOptions.push_back("-oro,nosuid,noexec,noatime");
	this->fuseOptions.push_back("-o");
	this->fuseOptions.push_back(std::string("subtype=") + getProgramName(this->fuseOptions.front().c_str()));
	this->fuseOptions.push_back("-o");
	this->fuseOptions.push_back(std::string("fsname=") + this->sourcePath);
	daemon_type &daemon = daemon_type::instance();
	daemon.root().setRealPath(sourcePath.c_str());
	char** argv = toArgv(this->fuseOptions);
	daemon.run(this->fuseOptions.size(), argv);
	freeArgv(argv);
}

void Main::Init(const int argc, const char* argv[]) throw(Main::Result)
{
	const char* const programName = getProgramName(argv[0]);
	std::vector<const char*> arguments(argv + 1, argv + argc);
	if (std::find_if(arguments.begin(), arguments.end(), isHelp) != arguments.end())
	{
		showUsage(argv[0]);
		throw Result(0);
	}
	if (std::find_if(arguments.begin(), arguments.end(), isVersion) != arguments.end())
	{
		showVersion(argv[0]);
		throw Result(0);
	}
	if (!parseParameters(arguments, this->sourcePath))
	{
		const char *message = (this->sourcePath.empty()) ? ": missing originalpath argument" : ": missing mountpoint argument";
		std::cerr << programName << message << std::endl;
		throw Result(-1);
	}
	for (std::vector<const char*>::iterator it = arguments.begin(); it != arguments.end(); it++)
	{
		this->fuseOptions.push_back(*it);
	}
	if (sourcePath[sourcePath.size() - 1] == '/') sourcePath.erase(sourcePath.size() - 1);
	this->fuseOptions.insert(this->fuseOptions.begin(), argv[0]);
}

Main::Main()
{
}

Main::~Main()
{
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
		<< " options:" << std::endl
		<< "    originalpath           the path to mount from and filter for zip files" << std::endl;
}

void showVersion(const char *self)
{
#ifdef HAVE_CONFIG_H
	std::cerr << PACKAGE_NAME << " version: " << PACKAGE_VERSION << std::endl;
	std::cerr << "Report bugs to " << PACKAGE_BUGREPORT << std::endl;
#else
	std::cerr << self << " debug version." << std::endl;
#endif
	const char * argv[2] = { self, "-V" };
	daemon_type::instance().run(2, const_cast<char **>(argv));
}

bool parseParameters(std::vector<const char*> &args, std::string &path)
{
	bool hasMountPoint = false;
	bool hasMountSource = false;
	std::vector<const char*>::iterator itPath;
	for (std::vector<const char*>::iterator it = args.begin(); it != args.end(); it++)
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

char* toArgv_convert(std::string value)
{
	char* newValue = new char[value.size() + 1];
	newValue[value.size()] = 0;
	strncpy(newValue, value.c_str(), value.size());
	return newValue;
}

char** toArgv(std::vector<std::string>& arguments)
{
	char** argv = new char*[arguments.size() + 1];
	char** argvIt = argv;
	for (std::vector<std::string>::iterator argIt = arguments.begin(); argIt != arguments.end(); argIt++, argvIt++)
	{
		*argvIt = toArgv_convert(*argIt);
	}
	*argvIt = NULL;
	return argv;
}

void freeArgv(char** argv)
{
	char** argvIt = argv;
	while (*argvIt != NULL)
	{
		delete[] *argvIt;
		argvIt++;
	}
	delete[] argv;
}
