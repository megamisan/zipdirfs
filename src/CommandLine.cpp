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

#include "CommandLine.h"
#include <boost/program_options.hpp>
#include <fstream>

namespace po = boost::program_options;

po::options_description CommandLine::cmdline;
po::options_description CommandLine::config;
po::options_description CommandLine::help;
po::positional_options_description CommandLine::positional;
po::variables_map CommandLine::values;
std::string CommandLine::config_file;

class IRegisterEntry
{
protected:
	IRegisterEntry *next;
public:
	IRegisterEntry();
	~IRegisterEntry();
	virtual IRegisterEntry* operator()();
	void Append(IRegisterEntry* entry);
};

class RegisterOptionEntry: public IRegisterEntry
{
private:
	po::options_description &(*initOptionsFunc)();
	CommandLine::OptionsKind kind;
public:
	RegisterOptionEntry(
			boost::program_options::options_description &(*initOptionsFunc)(),
			CommandLine::OptionsKind kind);
	IRegisterEntry* operator()();
};

class RegisterPositionalEntry: public IRegisterEntry
{
private:
	char *name;
	int count;
public:
	RegisterPositionalEntry(char *name, int count);
	IRegisterEntry* operator()();
};

IRegisterEntry headCL;
IRegisterEntry *first = NULL;

bool clInitialized = false;

CommandLine::CommandLine()
{
}

CommandLine::~CommandLine()
{
}

void CommandLine::init(int argc, char **argv)
{
	if (!clInitialized)
	{
		headCL.Append(first);
		first = NULL;
		IRegisterEntry *current = headCL();
		if (current != NULL)
			while ((current = (*current)()) != NULL)
				;
		po::options_description outputOptions("Output options");
		outputOptions.add_options()(
				"output-file,O",
				po::value<std::string>()->default_value(
						"-"), "output file name. '-' for standard output.");
		AddOptions(outputOptions, OPT_CLI_HELP);
		clInitialized = true;
	}
	po::store(
			po::command_line_parser(argc, argv). options(CommandLine::cmdline).positional(
					CommandLine::positional).run(), CommandLine::values);
	po::notify(CommandLine::values);
	std::ifstream ifs(CommandLine::config_file.c_str());
	if (ifs)
	{
		po::store(po::parse_config_file(ifs, CommandLine::config),
				CommandLine::values);
		po::notify(CommandLine::values);
	}
}

void CommandLine::AddOptions(po::options_description & desc, OptionsKind kind)
{
	if (kind & CommandLine::OPT_CLI)
		CommandLine::cmdline.add(desc);
	if (kind & CommandLine::OPT_CONFIG)
		CommandLine::config.add(desc);
	if (kind & CommandLine::OPT_HELP)
		CommandLine::help.add(desc);
}

CommandLine::Register::Register(
		boost::program_options::options_description &(*initOptionsFunc)(),
		OptionsKind kind)
{
	//	CommandLine::AddOptions(initOptionsFunc(), kind);
	IRegisterEntry *entry = new RegisterOptionEntry(initOptionsFunc, kind);
	if (first == NULL)
		first = entry;
	else
		first->Append(entry);
}

void CommandLine::AddOptions(const char *name, int count)
{
	CommandLine::positional.add(name, count);
}

const po::variable_value & CommandLine::get_Item(const std::string name)
{
	return CommandLine::values[name];
}

CommandLine::Register::Register(const char *name, int count)
{
	//	CommandLine::AddOptions(name, count);
	IRegisterEntry *entry = new RegisterPositionalEntry(
			const_cast<char*> (name), count);
	if (first == NULL)
		first = entry;
	else
		first->Append(entry);
}

std::string *CommandLine::get_ConfigFileRef()
{
	return &CommandLine::config_file;
}

std::ostream & CommandLine::ShowHelp(std::ostream &output)
{
	return (output << CommandLine::help);
}

IRegisterEntry * IRegisterEntry::operator ()()
{
	return this->next;
}

IRegisterEntry * RegisterOptionEntry::operator ()()
{
	CommandLine::AddOptions(this->initOptionsFunc(), this->kind);
	return IRegisterEntry::operator()();
}

RegisterOptionEntry::RegisterOptionEntry(
		boost::program_options::options_description & (*initOptionsFunc)(),
		CommandLine::OptionsKind kind) :
	IRegisterEntry(), initOptionsFunc(initOptionsFunc), kind(kind)
{
}

IRegisterEntry::~IRegisterEntry()
{
	if (this->next)
		delete this->next;
}

RegisterPositionalEntry::RegisterPositionalEntry(char *name, int count) :
	IRegisterEntry(), name(name), count(count)
{
}

void IRegisterEntry::Append(IRegisterEntry *entry)
{
	IRegisterEntry *last = this;
	IRegisterEntry *current = this->next;
	while (current != NULL)
	{
		last = current;
		current = current->next;
	}
	last->next = entry;
}

IRegisterEntry * RegisterPositionalEntry::operator ()()
{
	CommandLine::AddOptions(name, count);
	return IRegisterEntry::operator()();
}

IRegisterEntry::IRegisterEntry() :
	next(NULL)
{
}

int CommandLine::get_ItemCount(const std::string name)
{
	return CommandLine::values.count(name);
}

