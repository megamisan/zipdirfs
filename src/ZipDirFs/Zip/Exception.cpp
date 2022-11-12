/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Zip/Exception.h"
#include <cstring>

namespace ZipDirFs::Zip
{
	Exception::Exception(const std::string& s, const std::string& m) _GLIBCXX_USE_NOEXCEPT
		: Exception(s, m, 0)
	{
	}
	Exception::Exception(const std::string& s, const std::string& m, const int zipErrorCode,
		const int zipSystemCode) _GLIBCXX_USE_NOEXCEPT
		: Exception(s, m, (zipErrorCode << 8) + zipSystemCode)
	{
	}
	Exception::Exception(const std::string& s, const std::string& m,
		const int errorno) _GLIBCXX_USE_NOEXCEPT : source(s),
												   message(m),
												   errorno(errorno)
	{
	}

	Exception::~Exception() {}

	const char* Exception::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT
	{
		return message.c_str();
	}

	int Exception::code() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT { return errorno; }

	Exception Exception::fromErrorno(const std::string& s, int err) _GLIBCXX_USE_NOEXCEPT
	{
		return Exception(s, std::strerror(err), err);
	}

	std::ostream& operator<<(std::ostream& out, const Exception& ex)
	{
		return out << ex.source << ": " << ex.message;
	}

	const int zipErrorCode(const int code) _GLIBCXX_USE_NOEXCEPT { return code >> 8; }

	const int zipSystemCode(const int code) _GLIBCXX_USE_NOEXCEPT { return code & 255; }

} // namespace ZipDirFs::Zip
