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
	 * @brief A proxy to an @link ZipDirFs::Containers::Helpers::EntryListProxy Entry List Proxy @endlink
	 * @remarks Used to change the underlying helper for all references.
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class EntryListProxyProxy : public ZipDirFs::Containers::Helpers::EntryListProxy
	{
	public:
		EntryListProxyProxy(std::unique_ptr<ZipDirFs::Containers::Helpers::EntryListProxy>&&);
		void swap(std::unique_ptr<ZipDirFs::Containers::Helpers::EntryListProxy>&);
		iterator begin();
		iterator end();
		iterator insert(iterator, const key_type&, const mapped_type&);
		iterator erase(iterator);
		mapped_type find(const key_type& name) const;

	protected:
	private:
		std::unique_ptr<ZipDirFs::Containers::Helpers::EntryListProxy> entryListProxy;
	};

} // namespace ZipDirFs::Components

#endif // ZIPDIRFS_COMPONENTS_ENTRYLISTPROXYPROXY_H
