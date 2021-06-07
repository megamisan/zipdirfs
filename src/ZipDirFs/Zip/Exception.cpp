/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Zip/Exception.h"
#include <cstring>

namespace ZipDirFs::Zip
{
	Exception::Exception(const std::string& s, const std::string& m) _GLIBCXX_USE_NOEXCEPT
		: source(s),
		  message(m)
	{
	}

	Exception::~Exception() {}

	const char* Exception::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT
	{
		return message.c_str();
	}

	Exception Exception::fromErrorno(const std::string& s, int err) _GLIBCXX_USE_NOEXCEPT
	{
		return Exception(s, std::strerror(err));
	}

	std::ostream& operator<<(std::ostream& out, const Exception& ex)
	{
		return out << ex.source << ": " << ex.message;
	}

} // namespace ZipDirFs::Zip
