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
