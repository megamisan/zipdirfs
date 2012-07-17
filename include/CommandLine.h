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

#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/positional_options.hpp>
#include <string>
#include <iostream>

class CommandLine
{
private:
	static boost::program_options::options_description cmdline;
	static boost::program_options::options_description config;
	static boost::program_options::options_description help;
	static boost::program_options::positional_options_description positional;
	static boost::program_options::variables_map values;
	static std::string config_file;
	CommandLine();
public:
	~CommandLine();
	enum OptionsKind
	{
		OPT_NONE = 0,
		OPT_CLI = 1,
		OPT_CONFIG = 2,
		OPT_CLI_CONFIG = 3,
		OPT_HELP = 4,
		OPT_CLI_HELP = 5,
		OPT_CONFIG_HELP = 6,
		OPT_ALL = 7
	};
	static void AddOptions (boost::program_options::options_description& desc,
							OptionsKind kind);
	static void AddOptions (const char* name, int count);
	class Register
	{
	public:
		Register (
			boost::program_options::options_description & (*initOptionsFunc) (),
			OptionsKind kind);
		Register (const char* name, int count);
	};
	static void init (int argc, char * *argv);
	static const boost::program_options::variable_value& get_Item (
		const std::string name);
	static std::string* get_ConfigFileRef();
	static std::ostream& ShowHelp (std::ostream& output);
	static int get_ItemCount (const std::string name);
};

#endif /* COMMANDLINE_H_ */
