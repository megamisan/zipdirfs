/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_COMPONENTS_CHANGEDPROXY_H
#define ZIPDIRFS_COMPONENTS_CHANGEDPROXY_H

#include "ZipDirFs/Containers/Helpers/Changed.h"
#include <ctime>
#include <memory>

namespace ZipDirFs::Components
{
	class ChangedProxy : public ZipDirFs::Containers::Helpers::Changed
	{
	public:
		ChangedProxy(std::unique_ptr<ZipDirFs::Containers::Helpers::Changed>&&);
		bool operator()();
		operator std::time_t() const;
		void swap(std::unique_ptr<ZipDirFs::Containers::Helpers::Changed>&);

	protected:
	private:
		std::unique_ptr<ZipDirFs::Containers::Helpers::Changed> changed;
	};

} // namespace ZipDirFs::Components

#endif // ZIPDIRFS_COMPONENTS_CHANGEDPROXY_H
