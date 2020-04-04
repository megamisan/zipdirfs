/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_COMPONENTS_NATIVECHANGED_H
#define ZIPDIRFS_COMPONENTS_NATIVECHANGED_H

#include "ZipDirFs/Containers/Helpers/Changed.h"
#include <ctime>

namespace boost::filesystem
{
	class path;
}

namespace ZipDirFs::Components
{
	class NativeChanged : public ZipDirFs::Containers::Helpers::Changed
	{
	public:
		NativeChanged(const boost::filesystem::path&);
		bool operator()();
		operator std::time_t() const;

	protected:
		const boost::filesystem::path& path;
		std::time_t lastChanged;
	private:
	};

} // namespace ZipDirFs::Components

#endif // ZIPDIRFS_COMPONENTS_NATIVECHANGED_H
