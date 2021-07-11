/*
 * Copyright © 2012-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "Options.h"
#include "Main.h"
#include <algorithm>
#include <numeric>

Options::Options(int argc, const char* argv[]) : _self(*argv)
{
	argc--;
	argv++;
	_arguments.reserve(argc);
	while (argc)
	{
		this->_arguments.push_back(Options::string(*argv));
		argc--;
		argv++;
	}
}

Options::~Options() {}

void Options::addHandler(Options::string argument, Options::handler handler)
{
	this->directHandlers.insert(Options::handlerMap::value_type(argument, handler));
}

void Options::parseArguments()
{
	bool hasMountPoint = false;
	bool hasMountSource = false;
	bool previousIsOptionArgument = false;
	for (auto it = this->_arguments.begin(), endIt = this->_arguments.end(); it != endIt; ++it)
	{
		if (it->empty())
		{
			continue;
		}
		if (*it == "-o")
		{
			previousIsOptionArgument = true;
		}
		else if (previousIsOptionArgument || (it->length() > 2 && it->substr(0, 2) == "-o"))
		{
			explodeMountOptions(previousIsOptionArgument ? *it : it->substr(2));
			previousIsOptionArgument = false;
		}
		else if ((*it)[0] == '-' && it->length() > 1)
		{
			executeHandler(it->substr((it->length() > 2 && (*it)[1] == '-') ? 2 : 1));
			_unknownArguments.push_back(*it);
		}
		else
		{
			if (!hasMountPoint)
			{
				this->_mountPoint = *it;
				hasMountPoint = true;
			}
			else if (!hasMountSource)
			{
				this->_sourcePath.swap(this->_mountPoint);
				this->_mountPoint = *it;
				hasMountSource = true;
			}
			else
			{
				this->_unknownArguments.push_back(*it);
			}
		}
	}
}

void Options::explodeMountOptions(const Options::string& options)
{
	std::string::size_type start = 0, end, equal;
	while (start != std::string::npos)
	{
		end = options.find(',', start);
		std::string option =
			options.substr(start, (end == std::string::npos ? options.length() : end) - start);
		equal = option.find('=');
		equal = equal == std::string::npos ? option.length() : equal;
		setOption(option.substr(0, equal), equal < option.length() ? option.substr(equal + 1) : "");
		start = end == std::string::npos ? end : (end + 1);
	}
}

void Options::executeHandler(const Options::string& option) const
{
	auto handlerIt = this->directHandlers.find(option);
	if (handlerIt != this->directHandlers.end())
	{
		handlerIt->second(*this);
	}
}

bool Options::hasArgument(const string& name)
{
	return std::find(_unknownArguments.begin(), _unknownArguments.end(), name)
		!= _unknownArguments.end();
}

bool Options::hasOption(const string& name)
{
	return _mountOptions.find(name) != _mountOptions.end();
}

const Options::string& Options::getOption(const string& name)
{
	return _mountOptions[name];
}

void Options::setOption(const string& name, const string& value)
{
	_mountOptions[name] = value;
}

void Options::unsetOption(const string& name)
{
	_mountOptions.erase(name);
}

const Options::stringVector Options::makeArguments() const
{
	stringVector arguments;
	arguments.push_back(_self);
	arguments.push_back(_mountPoint);
	if (_mountOptions.size())
	{
		auto it = _mountOptions.begin();
		arguments.push_back("-o"
			+ std::accumulate(std::next(it), _mountOptions.end(),
				it->first + (it->second.length() ? "=" + it->second : ""),
				[](const std::string& a, const decltype(_mountOptions)::value_type& b)
				{ return a + "," + b.first + (b.second.length() ? "=" + b.second : ""); }));
	}
	std::copy(_unknownArguments.begin(), _unknownArguments.end(),
		std::back_insert_iterator<decltype(arguments)>(arguments));
	return arguments;
}
