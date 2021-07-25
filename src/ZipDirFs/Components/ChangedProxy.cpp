/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Components/ChangedProxy.h"

namespace ZipDirFs::Components
{
	ChangedProxy::ChangedProxy(std::unique_ptr<ZipDirFs::Containers::Helpers::Changed>&& changed) :
		changed(std::move(changed))
	{
	}

	bool ChangedProxy::operator()() { return (*changed)(); }

	ChangedProxy::operator std::time_t() const { return *changed; }

	void ChangedProxy::swap(std::unique_ptr<ZipDirFs::Containers::Helpers::Changed>& other)
	{
		changed.swap(other);
	}
} // namespace ZipDirFs::Components
