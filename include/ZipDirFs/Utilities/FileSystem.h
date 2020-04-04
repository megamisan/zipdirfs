/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_UTILITIES_FILESYSTEM_H
#define ZIPDIRFS_UTILITIES_FILESYSTEM_H

#include "ZipDirFs/Containers/EntryIterator.h"
#include <ctime>
#include <functional>
#include <sys/stat.h>

namespace boost::filesystem
{
	struct path;
	struct file_status;
}

namespace ZipDirFs::Utilities
{
	using namespace boost::filesystem;
	using ZipDirFs::Containers::EntryIterator;

	class FileSystem
	{
	public:
		static void reset();
		static std::function<std::time_t(const path&)> last_write_time;
		static std::function<EntryIterator(const path&)> directory_iterator_from_path;
		static std::function<EntryIterator()> directory_iterator_end;
		static std::function<file_status(const path&)> status;

		template <typename Func>
		inline static void assign(std::function<Func>& t, Func* f)
		{
			t = std::function<Func>(f);
		}

	private:
		FileSystem();
	};

} // namespace ZipDirFs::Utilities

#endif // ZIPDIRFS_UTILITIES_FILESYSTEM_H
