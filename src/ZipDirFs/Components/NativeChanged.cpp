/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Components/NativeChanged.h"
#include "ZipDirFs/Utilities/FileSystem.h"

namespace ZipDirFs::Components
{
	using ZipDirFs::Utilities::FileSystem;

	NativeChanged::NativeChanged(const boost::filesystem::path& p) : path(p), lastChanged{0}
	{
	}

	bool NativeChanged::operator()()
	{
		auto lastWrite = FileSystem::last_write_time(path);
		bool result = lastWrite != lastChanged;
		lastChanged = lastWrite;
		return result;
	}

	NativeChanged::operator std::time_t() const { return lastChanged; }

} // namespace ZipDirFs::Components
