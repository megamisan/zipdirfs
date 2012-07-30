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
