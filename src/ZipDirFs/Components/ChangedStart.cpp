/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Components/ChangedStart.h"

namespace ZipDirFs::Components
{
	ChangedStart::ChangedStart(std::function<std::time_t()>&& firstValue,
		std::function<std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed>()>&& buildReal,
		std::shared_ptr<ChangedProxy>& target) :
		lastChanged(0),
		called(false), firstValue(std::move(firstValue)), buildReal(std::move(buildReal)),
		target(target)
	{
	}

	bool ChangedStart::operator()()
	{
		std::unique_ptr<::ZipDirFs::Containers::Helpers::Changed> newChanged(buildReal());
		bool result = (*newChanged)();
		target->swap(newChanged);
		return result;
	}

	ChangedStart::operator std::time_t() const
	{
		if (!called)
		{
			*const_cast<std::time_t*>(&lastChanged) = firstValue();
			*const_cast<bool*>(&called) = true;
		}
		return lastChanged;
	}
} // namespace ZipDirFs::Components
