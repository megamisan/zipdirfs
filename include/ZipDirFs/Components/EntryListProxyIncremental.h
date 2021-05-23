/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_COMPONENTS_ENTRYLISTPROXYINCREMENTAL_H
#define ZIPDIRFS_COMPONENTS_ENTRYLISTPROXYINCREMENTAL_H

#include "ZipDirFs/Containers/Helpers/EntryListProxy.h"
#include <ctime>
#include <memory>
#include <functional>

namespace ZipDirFs::Components
{
	class EntryListProxyIncremental : public ZipDirFs::Containers::Helpers::EntryListProxy
	{
		using ELP = ::ZipDirFs::Containers::Helpers::EntryListProxy;

	public:
		EntryListProxyIncremental(std::function<mapped_type(const key_type&)>&&, std::shared_ptr<ELP>&&);
		const std::shared_ptr<ELP>& get() const;
		iterator begin();
		iterator end();
		iterator insert(iterator, const key_type&, const mapped_type&);
		iterator erase(iterator);
		mapped_type find(const key_type& name) const;

	protected:
	private:
		const std::shared_ptr<ELP> entryListProxy;
		const std::function<mapped_type(const key_type&)> resolve;
	};
} // namespace ZipDirFs::Components

#endif // ZIPDIRFS_COMPONENTS_ENTRYLISTPROXYINCREMENTAL_H
