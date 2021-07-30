/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_COMPONENTS_ENTRYLISTPROXYPROXY_H
#define ZIPDIRFS_COMPONENTS_ENTRYLISTPROXYPROXY_H

#include "ZipDirFs/Containers/Helpers/EntryListProxy.h"
#include <ctime>
#include <memory>

namespace ZipDirFs::Components
{
	/**
	 * @brief A proxy to an @link ZipDirFs::Containers::Helpers::EntryListProxy Entry List Proxy
	 * @endlink
	 * @remarks Used to change the underlying helper for all references.
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class EntryListProxyProxy : public ZipDirFs::Containers::Helpers::EntryListProxy
	{
	public:
		typedef std::unique_ptr<::ZipDirFs::Containers::Helpers::EntryListProxy> parent_ptr;
		EntryListProxyProxy(parent_ptr&&);
		void swap(parent_ptr&);
		iterator begin();
		iterator end();
		iterator insert(iterator, const key_type&, const mapped_type&);
		iterator erase(iterator);
		mapped_type find(const key_type& name) const;

	protected:
	private:
		parent_ptr entryListProxy;
	};

} // namespace ZipDirFs::Components

#endif // ZIPDIRFS_COMPONENTS_ENTRYLISTPROXYPROXY_H
