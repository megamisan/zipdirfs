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
#ifndef MAIN_H
#define MAIN_H

#include <string>
#include <vector>

class Main
{
	public:
		struct Result
		{
			const int result;
			Result(int res) : result(res) {}
			Result(const Result &res) : result(res.result) {}
		};
		Main();
		virtual ~Main();
		void Init(const int argc, const char* argv[]) throw(Result);
		void Run() throw(Result);
		inline const std::string getSourcePath() { return this->sourcePath; }
	protected:
	private:
		std::string sourcePath;
		std::vector<std::string> fuseOptions;
};

extern Main application;

#endif // MAIN_H
