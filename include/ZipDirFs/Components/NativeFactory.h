/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_COMPONENTS_NATIVEFACTORY_H
#define ZIPDIRFS_COMPONENTS_NATIVEFACTORY_H

#include "ZipDirFs/Containers/Helpers/Factory.h"
#include "ZipDirFs/Utilities/FileSystem.h"
#include "ZipDirFs/Zip/Exception.h"
#include "ZipDirFs/Zip/Lib.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Components
{
	using namespace ::boost::filesystem;
	using ::ZipDirFs::Utilities::FileSystem;
	using ::ZipDirFs::Zip::Exception;
	using ::ZipDirFs::Zip::Lib;

	template <class Directory, class Symlink, class Zip>
	class NativeFactory : public ::ZipDirFs::Containers::Helpers::Factory
	{
	public:
		NativeFactory(const path& path) : path(path) {}
		const mapped_type create(const key_type& name)
		{
			auto fullPath = path / name;
			file_status status = FileSystem::status(fullPath);
			if (is_directory(status))
			{
				return new Directory(fullPath);
			}
			if (is_regular_file(status))
			{
				bool zip = true;
				try
				{
					auto data = Lib::open(fullPath);
					Lib::close(data);
				}
				catch (Exception e)
				{
					zip = false;
				}
				return zip ? (mapped_type) new Zip(fullPath) :
							   (mapped_type) new Symlink(fullPath);
			}
			if (is_symlink(status))
			{
				return new Symlink(fullPath);
			}
			return nullptr;
		}
		void destroy(const mapped_type& item) { delete item; }

	protected:
		const struct path& path;
	};

} // namespace ZipDirFs::Components

#endif // ZIPDIRFS_COMPONENTS_NATIVEFACTORY_H
