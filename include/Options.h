/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <vector>
#include <map>

class Options
{
	typedef std::string string;
	typedef void (*const handler)(Options&);
	typedef std::map<string, handler> handlerMap;
public:
	typedef std::vector<string> stringVector;
	typedef std::map<string, string> stringMap;
	Options(int argc, const char* argv[]);
	~Options();
	void addHandler(string argument, handler handler);
	void parseArguments();
	const string& self() const { return this->self_; }
	const string& sourcePath() const { return this->sourcePath_; }
	const string& mountPoint() const { return this->mountPoint_; }
	const stringVector& arguments() const { return this->arguments_; }
	const stringVector& unknownArguments() const { return this->unknownArguments_; }
	const stringVector& fuseArguments() const { return this->fuseArguments_; }
	const stringMap& mountOptions() const { return this->mountOptions_; }
protected:
	void explodeMountOptions(string option);
	void executeHandler(string option);
private:
	stringVector arguments_;
	stringVector unknownArguments_;
	stringVector fuseArguments_;
	stringMap mountOptions_;
	string self_;
	string sourcePath_;
	string mountPoint_;
	handlerMap directHandlers;
};

#endif // OPTIONS_H
