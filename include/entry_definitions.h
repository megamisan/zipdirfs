/*
 * Copyright © 2012 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
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
 *
 * $Id$
 */
#ifndef ENTRY_DEFINITIONS_H_INCLUDED
#define ENTRY_DEFINITIONS_H_INCLUDED

#include "DefaultPermission.h"
#include "SystemDirectoryFactory.h"
#include "SystemDirectoryTime.h"
#include "DirectoryNode.h"
#include "LinkFactory.h"
#include "LinkTime.h"
#include "LinkBuffer.h"
#include "LinkNode.h"
#include <fusekit/basic_entry.h>
#include <fusekit/basic_directory.h>
#include <fusekit/no_buffer.h>

template <class Derived> class SystemDirectory : public DirectoryNode<SystemDirectoryFactory<>, Derived> {};
typedef fusekit::basic_directory<SystemDirectory, SystemDirectoryTime, DefaultDirectoryPermission> system_directory;

template <class Derived> class Link : public LinkNode<LinkFactory<>, Derived> {};
typedef fusekit::basic_entry<LinkTime, DefaultLinkPermission, LinkBuffer, Link, S_IFLNK> wrapper_link;


#endif // ENTRY_DEFINITIONS_H_INCLUDED
