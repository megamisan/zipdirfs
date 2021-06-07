/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIP_EXCEPTION_H
#define ZIPDIRFS_ZIP_EXCEPTION_H

#include <exception>
#include <ostream>
#include <string>

namespace ZipDirFs::Zip
{
	/**
	 * @brief An exception thrown when accessing zip archive
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class Exception : public std::exception
	{
	public:
		Exception(const std::string&, const std::string&) _GLIBCXX_USE_NOEXCEPT;
		~Exception() _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT;
		const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT;
		static Exception fromErrorno(const std::string& s, int) _GLIBCXX_USE_NOEXCEPT;

	protected:
	private:
		const std::string source;
		const std::string message;
		friend std::ostream& operator<<(std::ostream&, const Exception&);
	};

	std::ostream& operator<<(std::ostream&, const Exception&);

} // namespace ZipDirFs::Zip

#endif // ZIPDIRFS_ZIP_EXCEPTION_H
