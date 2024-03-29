/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_COMPONENTS_CHANGEDSTART_H
#define ZIPDIRFS_COMPONENTS_CHANGEDSTART_H

#include "ZipDirFs/Components/ChangedProxy.h"
#include "ZipDirFs/Containers/Helpers/Changed.h"
#include <ctime>
#include <functional>
#include <memory>
#include <mutex>

namespace ZipDirFs::Components
{
	/**
	 * @brief A @link ZipDirFs::Container::Helpers::Changed Changed @endlink helper for initial state
	 * @remarks Enables access to real file time before using the final @link ZipDirFs::Container::Helpers::Changed Changed @endlink helper.
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class ChangedStart : public ZipDirFs::Containers::Helpers::Changed
	{
	public:
		ChangedStart(std::function<std::time_t()>&&,
			std::function<std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed>()>&&,
			std::shared_ptr<ChangedProxy>&);
		bool operator()();
		operator std::time_t() const;

	protected:
		std::mutex access;
		std::time_t lastChanged;
		bool called;

	private:
		std::function<std::time_t()> firstValue;
		std::function<std::unique_ptr<ZipDirFs::Containers::Helpers::Changed>()> buildReal;
		std::shared_ptr<ChangedProxy>& target;
	};
} // namespace ZipDirFs::Components

#endif // ZIPDIRFS_COMPONENTS_CHANGEDSTART_H
