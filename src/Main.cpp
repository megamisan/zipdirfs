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

class CommandArguments
{
	const std::vector<std::string> arguments;

public:
	const int argc() const;
	const std::vector<const char*> argv() const;
	CommandArguments(const Options&);
	~CommandArguments();
};

std::string getProgramName(const std::string& self);
void showUsage(const Options&);
void showVersion(const Options&);
char** toArgv(std::vector<std::string>& arguments);
void freeArgv(char** argv);

int main(const int argc, const char** argv)
{
	Main application(argc, argv);
	try
	{
		application.init();
		application.run();
	}
	catch (Main::Result res)
	{
		return res.result;
	}
	return 0;
}

void Main::run()
{
	options.setOption("subtype", getProgramName(getProgramName(options.self())));
	options.setOption("fsname", options.sourcePath());
	daemon_type& daemon = daemon_type::instance();
	daemon.root() =
		std::unique_ptr<fusekit::entry>(new ZipDirFs::Fuse::NativeDirectory(options.sourcePath()));
	CommandArguments arguments(options);
	daemon.run(arguments.argc(), (char**)&(arguments.argv()[0]), false);
}

void Main::init()
{
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
	options.setOption("ro", "");
	options.setOption("nosuid", "");
	options.setOption("noexec", "");
	options.setOption("noatime", "");
}

Main::Main(const int argc, const char** argv) : options(argc, argv) {}

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

void showUsage(const Options& options)
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

void showVersion(const Options& options)
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

CommandArguments::CommandArguments(const Options& options) :
	arguments(std::move(options.makeArguments()))
{
}

const int CommandArguments::argc() const
{
	return arguments.size();
}

const std::vector<const char*> CommandArguments::argv() const
{
	std::vector<const char*> argv;
	std::transform(arguments.begin(), arguments.end(),
		std::back_insert_iterator<decltype(argv)>(argv),
		[](const std::string& s) { return s.c_str(); });
	return argv;
}

CommandArguments::~CommandArguments() {}
