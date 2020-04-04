/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_COMPONENTS_NATIVECHANGED_H
#define TEST_ZIPDIRFS_COMPONENTS_NATIVECHANGED_H

#include "ZipDirFs/Components/NativeChanged.h"

namespace Test::ZipDirFs::Components
{
	using NativeChangedBase = ::ZipDirFs::Components::NativeChanged;

	struct NativeChangedAccess : public NativeChangedBase
	{
		void set_lastChanged(std::time_t t) { this->lastChanged = t; }
	};
} // namespace Test::ZipDirFs::Components

#endif // TEST_ZIPDIRFS_COMPONENTS_NATIVECHANGED_H
