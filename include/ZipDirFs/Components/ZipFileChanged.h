/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_COMPONENTS_ZIPFILECHANGED_H
#define ZIPDIRFS_COMPONENTS_ZIPFILECHANGED_H

#include "ZipDirFs/Containers/Helpers/Changed.h"
#include <bitset>
#include <ctime>
#include <string>

namespace boost::filesystem
{
	class path;
}

namespace ZipDirFs::Components
{
	/**
	 * @brief A @link ZipDirFs::Container::Helpers::Changed Changed @endlink helper for entry in zip archive
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class ZipFileChanged : public ZipDirFs::Containers::Helpers::Changed
	{
	public:
		ZipFileChanged(const boost::filesystem::path&, const std::string&);
		bool operator()();
		operator std::time_t() const;
		static const std::time_t getTime(const boost::filesystem::path&, const std::string&);

	protected:
		const boost::filesystem::path& path;
		const std::string& item;
		std::time_t lastPathChanged;
		std::time_t lastItemChanged;
		std::bitset<2> state;

	private:
	};

} // namespace ZipDirFs::Components

#endif // ZIPDIRFS_COMPONENTS_ZIPFILECHANGED_H
