/*
 * Copyright © 2012-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "Main.h"
#include "Options.h"
#include "ZipDirFs/Fuse/EntryProxy.h"
#include "ZipDirFs/Fuse/NativeDirectory.h"
#if HAVE_CONFIG_H
#	include "config.h"
#endif
#include <algorithm>
#include <cstring>
#include <fusekit/daemon.h>
#include <iostream>
#include <vector>

typedef fusekit::daemon<ZipDirFs::Fuse::EntryProxy> daemon_type;

struct CommandArguments
{
	const int argc;
	const char* const* argv;
	CommandArguments(const std::vector<std::string>&);
	~CommandArguments();

private:
	static char* toArgv_convert(const std::string&);
	static char** toArgv(const std::vector<std::string>&);
};

Main application;

std::string getProgramName(const std::string& self);
void showUsage(Options&);
void showVersion(Options&);
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

void Main::Run()
{
	this->fuseOptions.push_back("-o");
	this->fuseOptions.push_back(
		std::string("subtype=") + getProgramName(this->fuseOptions.front()));
	this->fuseOptions.push_back("-o");
	this->fuseOptions.push_back(std::string("fsname=") + this->sourcePath);
	daemon_type& daemon = daemon_type::instance();
	daemon.root() =
		std::unique_ptr<fusekit::entry>(new ZipDirFs::Fuse::NativeDirectory(this->sourcePath));
	CommandArguments arguments(this->fuseOptions);
	daemon.run(arguments.argc, (char**)arguments.argv, false);
}

void Main::Init(const int argc, const char* argv[])
{
	Options options(argc, argv);
	options.addHandler("h", showUsage);
	options.addHandler("help", showUsage);
	options.addHandler("V", showVersion);
	options.addHandler("version", showVersion);
	options.parseArguments();

	if (options.sourcePath().empty() || options.mountPoint().empty())
	{
		const char* message = (options.mountPoint().empty()) ?
			  ": missing originalpath argument" :
			  ": missing mountpoint argument"; // Argument order is originalpath then mountpoint.
											 // Parse order is reversed. See
											 // Options::parseArguments();
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

	for (Options::stringVector::const_iterator it = options.fuseArguments().begin();
		 it != options.fuseArguments().end(); it++)
	{
		this->fuseOptions.push_back("-o");
		this->fuseOptions.push_back(*it);
	}

	for (Options::stringVector::const_iterator it = options.unknownArguments().begin();
		 it != options.unknownArguments().end(); it++)
	{
		this->fuseOptions.push_back(*it);
	}
}

Main::Main() {}

Main::~Main() {}

std::string getProgramName(const std::string& self)
{
	size_t pos = self.rfind('/');
	if (pos == std::string::npos)
	{
		pos = 0;
	}
	else
	{
		pos++;
	}
	return self.substr(pos);
}

void showUsage(Options& options)
{
	std::cerr << "real usage: " << options.self() << " originalpath mountpoint [options]"
			  << std::endl;
	const char* argv[2] = {options.self().c_str(), "-h"};
	daemon_type::instance().run(2, const_cast<char**>(argv));
	std::cerr << std::endl
			  << getProgramName(options.self()) << " options:" << std::endl
			  << "    originalpath           the path to mount from and filter for zip files"
			  << std::endl;
	throw Main::Result(0);
}

void showVersion(Options& options)
{
#ifdef HAVE_CONFIG_H
	std::cerr << PACKAGE_NAME << " version: " << PACKAGE_VERSION << std::endl;
	std::cerr << "Report bugs to " << PACKAGE_BUGREPORT << std::endl;
#else
	std::cerr << options.self() << " debug version." << std::endl;
#endif
	const char* argv[2] = {options.self().c_str(), "-V"};
	daemon_type::instance().run(2, const_cast<char**>(argv));
	throw Main::Result(0);
}

CommandArguments::CommandArguments(const std::vector<std::string>& value) :
	argc(value.size()), argv(toArgv(value))
{
}

CommandArguments::~CommandArguments()
{
	char* const* argvIt = (char* const*)argv;
	while (*argvIt != NULL)
	{
		delete[] * argvIt;
		argvIt++;
	}
	delete[] argv;
}

char* CommandArguments::toArgv_convert(const std::string& value)
{
	char* newValue = new char[value.size() + 1];
	newValue[value.size()] = 0;
	strncpy(newValue, value.c_str(), value.size());
	return newValue;
}

char** CommandArguments::toArgv(const std::vector<std::string>& arguments)
{
	char** argv = new char*[arguments.size() + 1];
	char** argvIt = argv;
	for (std::vector<std::string>::const_iterator argIt = arguments.begin();
		 argIt != arguments.end(); argIt++, argvIt++)
	{
		*argvIt = toArgv_convert(*argIt);
	}
	*argvIt = NULL;
	return argv;
}
