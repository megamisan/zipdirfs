#ifndef NOTFOUNDEXCEPTION_H
#define NOTFOUNDEXCEPTION_H

#include <exception>


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

#endif // NOTFOUNDEXCEPTION_H
