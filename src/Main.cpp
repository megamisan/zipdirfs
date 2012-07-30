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
#include "Options.h"
#include "zipdirfs/entry_definitions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <fusekit/daemon.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string.h>

typedef fusekit::daemon<zipdirfs::system_directory> daemon_type;

Main application;

std::string getProgramName(const std::string& self);
void showUsage(Options&) throw(Main::Result);
void showVersion(Options&) throw(Main::Result);
char** toArgv(std::vector<std::string>& arguments);
void freeArgv(char** argv);

int main(const int argc, const char** argv)
{
	try
	{
		application.Init(argc, argv);
		application.Run();
	}
	catch (Main::Result res)
	{
		return res.result;
	}
	return 0;
}

void Main::Run() throw(Main::Result)
{
	this->fuseOptions.push_back("-o");
	this->fuseOptions.push_back(std::string("subtype=") + getProgramName(this->fuseOptions.front()));
	this->fuseOptions.push_back("-o");
	this->fuseOptions.push_back(std::string("fsname=") + this->sourcePath);
	daemon_type &daemon = daemon_type::instance();
	daemon.root().setRealPath(this->sourcePath.c_str());
	char** argv = toArgv(this->fuseOptions);
	daemon.run(this->fuseOptions.size(), argv);
	freeArgv(argv);
}

void Main::Init(const int argc, const char* argv[]) throw(Main::Result)
{
	Options options(argc, argv);
	options.addHandler("h", showUsage);
	options.addHandler("help", showUsage);
	options.addHandler("V", showVersion);
	options.addHandler("version", showVersion);
	options.parseArguments();

	if (options.sourcePath().empty() || options.mountPoint().empty())
	{
		const char *message = (options.mountPoint().empty()) ? ": missing originalpath argument" : ": missing mountpoint argument"; // Argument order is originalpath then mountpoint. Parse order is reversed. See Options::parseArguments();
		std::cerr << getProgramName(options.self()) << message << std::endl;
		throw Result(-1);
	}

	this->sourcePath = options.sourcePath();

	this->fuseOptions.push_back(options.self());
	this->fuseOptions.push_back(options.mountPoint());

	Options::stringMap mountOptions(options.mountOptions());
	mountOptions["ro"] = "";
	mountOptions["nosuid"] = "";
	mountOptions["noexec"] = "";
	mountOptions["noatime"] = "";
	std::string mountArgument("-");
	mountArgument.reserve(2 + (mountOptions.size() << 3));
	for (Options::stringMap::iterator mit = mountOptions.begin(); mit != mountOptions.end(); mit++)
	{
		mountArgument.append(",");
		mountArgument.append(mit->first);
		if (!mit->second.empty())
		{
			mountArgument.append("=");
			mountArgument.append(mit->second);
		}
	}
	mountArgument[1] = 'o';
	this->fuseOptions.push_back(mountArgument);

	for (Options::stringVector::const_iterator it = options.fuseArguments().begin(); it != options.fuseArguments().end(); it++)
	{
		this->fuseOptions.push_back("-o");
		this->fuseOptions.push_back(*it);
	}

	for (Options::stringVector::const_iterator it = options.unknownArguments().begin(); it != options.unknownArguments().end(); it++)
	{
		this->fuseOptions.push_back(*it);
	}
}

Main::Main()
{
}

Main::~Main()
{
}

std::string getProgramName(const std::string& self)
{
	size_t pos = self.rfind('/');
	if (pos == std::string::npos) pos = 0;
	else pos++;
	return self.substr(pos);
}

void showUsage(Options& options) throw(Main::Result)
{
	std::cerr << "real usage: " << options.self() << " originalpath mountpoint [options]" << std::endl;
	const char * argv[2] = { options.self().c_str(), "-h" };
	daemon_type::instance().run(2, const_cast<char **>(argv));
	std::cerr << std::endl
		<< getProgramName(options.self())
		<< " options:" << std::endl
		<< "    originalpath           the path to mount from and filter for zip files" << std::endl;
	throw Main::Result(0);
}

void showVersion(Options& options) throw(Main::Result)
{
#ifdef HAVE_CONFIG_H
	std::cerr << PACKAGE_NAME << " version: " << PACKAGE_VERSION << std::endl;
	std::cerr << "Report bugs to " << PACKAGE_BUGREPORT << std::endl;
#else
	std::cerr << options.self() << " debug version." << std::endl;
#endif
	const char * argv[2] = { options.self().c_str(), "-V" };
	daemon_type::instance().run(2, const_cast<char **>(argv));
	throw Main::Result(0);
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
