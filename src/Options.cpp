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
#include "Options.h"
#include "Main.h"
#include <algorithm>

Options::Options(int argc, const char* argv[]) : arguments_(argc), self_(*argv)
{
	argc--; argv++;
	while (argc)
	{
		this->arguments_.push_back(Options::string(*argv));
		argc--; argv++;
	}
}

Options::~Options()
{
}

void Options::addHandler(Options::string argument, Options::handler handler)
{
	this->directHandlers.insert(Options::handlerMap::value_type(argument, handler));
}

void Options::parseArguments()
{
	bool hasMountPoint = false;
	bool hasMountSource = false;
	bool inValuedOption = false;
	std::string lastOption;
	for (Options::stringVector::iterator it = this->arguments_.begin(); it != this->arguments_.end(); it++)
	{
		if (it->empty()) continue; // TODO: Investigate bug
		if ((*it)[0] == '-')
		{
			if (inValuedOption && (lastOption != "-o"))
			{
				this->executeHandler(lastOption.substr(1));
				this->unknownArguments_.push_back(lastOption);
			}
			inValuedOption = false;
			if (it->size() == 2)
			{
				inValuedOption = true;
				lastOption = *it;
			}
			else if (it->size() > 2)
			{
				switch ((*it)[1])
				{
					case '-':
						this->executeHandler(it->substr(2));
						this->unknownArguments_.push_back(*it);
						break;
					case 'o':
						this->explodeMountOptions(it->substr(2));
						break;
					default:
						this->unknownArguments_.push_back(*it);
				}
			}
			continue;
		}
		if (inValuedOption)
		{
			if (lastOption == "-o")
			{
				if (std::find(this->fuseArguments_.begin(), this->fuseArguments_.end(), *it) == this->fuseArguments_.end()) this->fuseArguments_.push_back(*it);
			}
			else
			{
				this->unknownArguments_.push_back(lastOption.substr(1));
				this->unknownArguments_.push_back(*it);
			}
			inValuedOption = false;
			continue;
		}
		if (!hasMountPoint)
		{
			this->mountPoint_ = *it;
			hasMountPoint = true;
		}
		else if (!hasMountSource)
		{
			this->sourcePath_.swap(this->mountPoint_);
			this->mountPoint_ = *it;
			hasMountSource = true;
		}
		else
		{
			this->unknownArguments_.push_back(*it);
		}
	}
	if (inValuedOption && (lastOption != "-o"))
	{
		this->executeHandler(lastOption.substr(1));
		this->unknownArguments_.push_back(lastOption);
	}
}

void Options::explodeMountOptions(Options::string option)
{
	Options::string::iterator
		nameStart = option.begin(),
		nameEnd = option.begin(),
		valueStart = option.end();
	bool hasValue = false;
	for (Options::string::iterator it = option.begin(); it != option.end(); it++)
	{
		switch (*it)
		{
			case '=':
				nameEnd = it;
				hasValue = true;
				valueStart = it;
				valueStart++;
				break;
			case ',':
				if (hasValue)
				{
					if (nameStart != nameEnd) this->mountOptions_[Options::string(nameStart, nameEnd)] = Options::string(valueStart, it);
					hasValue = false;
				}
				else
				{
					if (nameStart != it) this->mountOptions_[Options::string(nameStart, it)] = "";
				}
				nameStart = it;
				nameStart++;
				nameEnd = nameStart;
				break;
		}
	}
	if (hasValue)
	{
		if (nameStart != nameEnd) this->mountOptions_[Options::string(nameStart, nameEnd)] = Options::string(valueStart, option.end());
	}
	else
	{
		if (nameStart != option.end()) this->mountOptions_[Options::string(nameStart, option.end())] = "";
	}
}

void Options::executeHandler(Options::string option)
{
	Options::handlerMap::iterator handlerIt = this->directHandlers.find(option);
	if (handlerIt != this->directHandlers.end())
	{
		handlerIt->second(*this);
	}
}
