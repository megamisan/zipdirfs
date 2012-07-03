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
#ifndef STREAM_INDENT_H
#define STREAM_INDENT_H

#include <ostream>

namespace zipdirfs
{
	struct _IndentChar
	{
		char c;
	};

	std::ostream& indent_inc (std::ostream&);
	std::ostream& indent_dec (std::ostream&);
	std::ostream& indent (std::ostream&);
	inline _IndentChar indent_char (char c)
	{
		_IndentChar ic = { c };
		return ic;
	}
	std::ostream& operator << (std::ostream&, _IndentChar);
}

#endif // STREAM_INDENT_H
