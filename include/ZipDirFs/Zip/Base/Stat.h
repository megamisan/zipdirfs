/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIP_BASE_STAT_H
#define ZIPDIRFS_ZIP_BASE_STAT_H

#include <ctime>
#include <string>

namespace ZipDirFs::Zip::Base
{
	/**
	 * @brief Helper class to represents a zip archive entry stats
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class Stat
	{
	public:
		Stat() = default;
		Stat(const std::uint64_t, const std::string&, const std::uint64_t, const std::time_t,
			const bool);
		const std::uint64_t getIndex() const;
		const std::string& getName() const;
		const std::uint64_t getSize() const;
		const std::time_t getChanged() const;
		const bool getCompressed() const;

	protected:
		std::uint64_t index;
		std::string name;
		std::uint64_t size;
		std::time_t changed;
		bool compressed;
	};

} // namespace ZipDirFs::Zip::Base

#endif // ZIPDIRFS_ZIP_BASE_STAT_H
