/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
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
	/**
	 * @brief A @link ZipDirFs::Container::Helpers::Changed Changed @endlink helper for real files and directories
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class NativeChanged : public ZipDirFs::Containers::Helpers::Changed
	{
	public:
		NativeChanged(const boost::filesystem::path&);
		bool operator()();
		operator std::time_t() const;
		static const std::time_t getTime(const boost::filesystem::path&);

	protected:
		const boost::filesystem::path& path;
		std::time_t lastChanged;

	private:
	};

} // namespace ZipDirFs::Components

#endif // ZIPDIRFS_COMPONENTS_NATIVECHANGED_H
