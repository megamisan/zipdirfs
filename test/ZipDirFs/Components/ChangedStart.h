/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_COMPONENTS_CHANGEDSTART_H
#define TEST_ZIPDIRFS_COMPONENTS_CHANGEDSTART_H

#include "ZipDirFs/Components/ChangedStart.h"

namespace Test::ZipDirFs::Components
{
	struct ChangedStartAccess : public ::ZipDirFs::Components::ChangedStart
	{
		static std::time_t get_lastChanged(const ::ZipDirFs::Components::ChangedStart& cs) {
			return reinterpret_cast<const ChangedStartAccess*>(&cs)->lastChanged;
		}
		static bool get_called(const ::ZipDirFs::Components::ChangedStart& cs) {
			return reinterpret_cast<const ChangedStartAccess*>(&cs)->called;
		}
		static void set_lastChanged(const ::ZipDirFs::Components::ChangedStart& cs, const std::time_t newValue) {
			*const_cast<std::time_t*>(&reinterpret_cast<const ChangedStartAccess*>(&cs)->lastChanged) = newValue;
		}
		static void set_called(const ::ZipDirFs::Components::ChangedStart& cs, const bool newValue) {
			*const_cast<bool*>(&reinterpret_cast<const ChangedStartAccess*>(&cs)->called) = newValue;
		}
	};
} // namespace Test::ZipDirFs::Components

#endif // TEST_ZIPDIRFS_COMPONENTS_CHANGEDSTART_H
