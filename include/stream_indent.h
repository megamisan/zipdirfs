#ifndef STREAM_INDENT_H
#define STREAM_INDENT_H

#include <ostream>

struct _IndentChar
{
	char c;
};

std::ostream& indent_inc(std::ostream&);
std::ostream& indent_dec(std::ostream&);
std::ostream& indent(std::ostream&);
inline _IndentChar indent_char(char c) { _IndentChar ic = { c }; return ic; }
std::ostream& operator << (std::ostream&, _IndentChar);

#endif // STREAM_INDENT_H
