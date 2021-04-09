/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_NOTFOUNDEXCEPTION_H
#define ZIPDIRFS_NOTFOUNDEXCEPTION_H

#include <exception>

namespace ZipDirFs
{
	/**
	 * \brief Exception thrown when an entry is not found.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class NotFoundException : public std::exception
	{
	public:
		/** Default constructor */
		NotFoundException();
		/** Default destructor */
		virtual ~NotFoundException() throw();
	protected:
	private:
	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_NOTFOUNDEXCEPTION_H
