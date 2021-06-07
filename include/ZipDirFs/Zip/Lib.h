/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIP_LIB_H
#define ZIPDIRFS_ZIP_LIB_H

#include "ZipDirFs/Zip/Base/Lib.h"
#include "ZipDirFs/Zip/Base/Stat.h"
#include <functional>
#include <string>

namespace boost::filesystem
{
	struct path;
}

namespace ZipDirFs::Zip
{
	using namespace boost::filesystem;

	/**
	 * @brief Global libzip call wrapper
	 * @remarks Used to help in executing unit tests and simplifying error handling from source library.
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class Lib
	{
	public:
		static void reset();
		/** Opens a Zip Archive */
		static std::function<Base::Lib*(const path&)> open;
		/** Closes a Zip Archive */
		static std::function<void(Base::Lib* const)> close;
		/** Retrieves the number of entries in Zip Archive */
		static std::function<std::uint64_t(Base::Lib* const) noexcept> get_num_entries;
		/** Retrieves the informations about a file in Zip Archive by name */
		static std::function<Base::Stat(Base::Lib* const, const std::string&)> stat;
		/** Retrieves the informations about a file in Zip Archive by index */
		static std::function<Base::Stat(Base::Lib* const, const std::uint64_t)> stat_index;
		/** Retrieves a file's name in Zip Archive by index */
		static std::function<std::string(Base::Lib* const, std::uint64_t)> get_name;
		/** Opens file in Zip Archive */
		static std::function<Base::Lib::File*(Base::Lib* const, std::uint64_t)> fopen_index;
		/** Reads data from file */
		static std::function<std::uint64_t(Base::Lib::File* const, void*, std::uint64_t)> fread;
		/** Closes file */
		static std::function<void(Base::Lib::File* const)> fclose;

		template <typename Func>
		inline static void assign(std::function<Func>& t, Func* f)
		{
			t = std::function<Func>(f);
		}
	};

} // namespace ZipDirFs::Zip

#endif // ZIPDIRFS_ZIP_LIB_H
