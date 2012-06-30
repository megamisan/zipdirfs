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
#include "stream_indent.h"
#include <string>

int getIndentIndex()
{
	static int index = 0;
	if (index == 0)
	{
		index = std::ios_base::xalloc();
	}
	return index;
}

int getIndentCharIndex()
{
	static int index = 0;
	if (index == 0)
	{
		index = std::ios_base::xalloc();
	}
	return index;
}

std::ostream& indent_inc(std::ostream& out)
{
	out.iword(getIndentIndex())++;
	return out;
}

std::ostream& indent_dec(std::ostream& out)
{
	out.iword(getIndentIndex())--;
	return out;
}

std::ostream& indent(std::ostream& out)
{
	char c = (char)out.iword(getIndentCharIndex());
	if (c == 0)
	{
		c = '\t';
	}
	out << std::string(out.iword(getIndentIndex()), c);
	return out;
}

std::ostream& operator << (std::ostream& out, _IndentChar ic)
{
	out.iword(getIndentCharIndex()) = ic.c;
	return out;
}
