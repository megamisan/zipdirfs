/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
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
 */
#ifndef DEFAULTPERMISSION_H
#define DEFAULTPERMISSION_H

#include <asm-generic/errno-base.h>
#include <sys/types.h>

namespace ZipDirFs
{
	/**
	 * \brief Represents some fixed permissions
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template < ::mode_t Permissions, class Derived>
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
}

#endif // DEFAULTPERMISSION_H
