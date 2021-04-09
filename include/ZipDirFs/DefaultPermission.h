/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_DEFAULTPERMISSION_H
#define ZIPDIRFS_DEFAULTPERMISSION_H

#include <asm-generic/errno-base.h>
#include <sys/types.h>

namespace ZipDirFs
{
	/**
	 * \brief Represents some fixed permissions
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template <::mode_t Permissions, class Derived>
	class DefaultPermission
	{
	public:
		DefaultPermission() {}
		virtual ~DefaultPermission() {}
		int access(int permission)
		{
			return (permission & (~Permissions)) ? -EACCES : 0;
		}
		int chmod(::mode_t)
		{
			return -EACCES;
		}
		int mode()
		{
			return Permissions;
		}
	protected:
	private:
	};

	/**
	 * \brief Fixed permissions for directories.
	 * @author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template <class Derived>
	class DefaultDirectoryPermission : public DefaultPermission<0555, Derived> {};

	/**
	 * \brief Fixed permissions for files.
	 * @author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template <class Derived>
	class DefaultFilePermission : public DefaultPermission<0444, Derived> {};

	/**
	 * \brief Fixed permissions for links.
	 * @author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template <class Derived>
	class DefaultLinkPermission : public DefaultPermission<0777, Derived> {};
} // namespace ZipDirFs

#endif // ZIPDIRFS_DEFAULTPERMISSION_H
