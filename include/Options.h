/*
 * Copyright © 2012-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef OPTIONS_H
#define OPTIONS_H

#include <map>
#include <string>
#include <vector>

/**
 * @brief Helper for parsing application options
 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
class Options
{
	typedef std::string string;
	typedef void (*const handler)(const Options&);
	typedef std::map<string, handler> handlerMap;

public:
	typedef std::vector<string> stringVector;
	typedef std::map<string, string> stringMap;
	Options(int argc, const char* argv[]);
	~Options();
	void addHandler(string argument, handler handler);
	void parseArguments();
	const string& self() const { return this->_self; }
	const string& sourcePath() const { return this->_sourcePath; }
	const string& mountPoint() const { return this->_mountPoint; }
	bool hasArgument(const string&);
	bool hasOption(const string&);
	const string& getOption(const string&);
	void setOption(const string&, const string&);
	void unsetOption(const string&);
	const stringVector makeArguments() const;

protected:
	void explodeMountOptions(const string& option);
	void executeHandler(const string& option) const;

private:
	stringVector _arguments;
	stringVector _unknownArguments;
	stringMap _mountOptions;
	string _self;
	string _sourcePath;
	string _mountPoint;
	handlerMap directHandlers;
};

#endif // OPTIONS_H
