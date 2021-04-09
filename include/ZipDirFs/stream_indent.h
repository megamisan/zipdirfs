/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_STREAM_INDENT_H
#define ZIPDIRFS_STREAM_INDENT_H

#include <ostream>

namespace ZipDirFs
{
	struct _IndentChar
	{
		char c;
	};

	std::ostream& indent_inc(std::ostream&);
	std::ostream& indent_dec(std::ostream&);
	std::ostream& indent(std::ostream&);
	inline _IndentChar indent_char(char c)
	{
		_IndentChar ic = { c };
		return ic;
	}
	std::ostream& operator << (std::ostream&, _IndentChar);
} // namespace ZipDirFs

#endif // ZIPDIRFS_STREAM_INDENT_H
