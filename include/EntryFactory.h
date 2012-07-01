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
 * $
 */
#ifndef ENTRYFACTORY_H
#define ENTRYFACTORY_H

#include <fusekit/entry.h>
#include <dirent.h>
#include <string>

class EntryFactory
{
	public:
		virtual ~EntryFactory();
		static fusekit::entry* newEntry(const dirent* dirEntry, const std::string& basePath);
	protected:
	private:
		EntryFactory();
};

#endif // ENTRYFACTORY_H