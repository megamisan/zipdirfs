/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_COMPONENTS_ZIPDIRECTORYENUMERATOR_H
#define ZIPDIRFS_COMPONENTS_ZIPDIRECTORYENUMERATOR_H

#include "ZipDirFs/Containers/EntryIterator.h"
#include "ZipDirFs/Containers/Helpers/Enumerator.h"
#include <string>

namespace boost::filesystem
{
	struct path;
}

namespace ZipDirFs::Components
{
	class ZipDirectoryEnumerator : public ZipDirFs::Containers::Helpers::Enumerator<std::string>
	{
		using iterator = ::ZipDirFs::Containers::EntryIterator;

	public:
		ZipDirectoryEnumerator(const boost::filesystem::path&, const std::string&);
		~ZipDirectoryEnumerator();
		void reset();
		void next();
		bool valid();
		const std::string& current();

	protected:
		const boost::filesystem::path& path;
		const std::string& item;
		iterator currentIt;
		iterator endIt;

	private:
	};

} // namespace ZipDirFs::Components

#endif // ZIPDIRFS_COMPONENTS_ZIPDIRECTORYENUMERATOR_H
