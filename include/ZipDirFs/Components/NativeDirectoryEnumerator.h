/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_COMPONENTS_NATIVEDIRECTORYENUMERATOR_H
#define ZIPDIRFS_COMPONENTS_NATIVEDIRECTORYENUMERATOR_H

#include "ZipDirFs/Containers/EntryIterator.h"
#include "ZipDirFs/Containers/Helpers/Enumerator.h"
#include <string>

namespace boost::filesystem
{
	struct path;
}

namespace ZipDirFs::Components
{
	/**
	 * @brief An @link ZipDirFs::Containers::Helpers::Enumerator Enumerator @endlink for real directories
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class NativeDirectoryEnumerator : public ZipDirFs::Containers::Helpers::Enumerator<std::string>
	{
		using iterator = ::ZipDirFs::Containers::EntryIterator;

	public:
		NativeDirectoryEnumerator(const boost::filesystem::path&);
		~NativeDirectoryEnumerator();
		void reset();
		void next();
		bool valid();
		const std::string& current();

	protected:
		const boost::filesystem::path& path;
		iterator currentIt;
		iterator endIt;

	private:
	};

} // namespace ZipDirFs::Components

#endif // ZIPDIRFS_COMPONENTS_NATIVEDIRECTORYENUMERATOR_H
