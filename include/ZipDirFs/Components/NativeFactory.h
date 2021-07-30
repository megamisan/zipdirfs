/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_COMPONENTS_NATIVEFACTORY_H
#define ZIPDIRFS_COMPONENTS_NATIVEFACTORY_H

#include "StateReporter.h"
#include "ZipDirFs/Containers/Helpers/Factory.h"
#include "ZipDirFs/Utilities/FileSystem.h"
#include "ZipDirFs/Zip/Exception.h"
#include "ZipDirFs/Zip/Factory.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Components
{
	using namespace ::boost::filesystem;
	using ::ZipDirFs::Utilities::FileSystem;
	using ::ZipDirFs::Zip::Exception;
	using ZipFactory = ::ZipDirFs::Zip::Factory;

	/**
	 * @brief A @link ZipDirFs::Containers::Helpers::Factory Factory @endlink for creating entries
	 * under a real directory
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 * @tparam Directory Implementation class for real directory.
	 * @tparam Symlink Implementation class for symlink to real file.
	 * @tparam Zip Implementation class for zip directory representing the archive root.
	 */
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
				return reportWrap(new Directory(fullPath));
			}
			if (is_regular_file(status))
			{
				mapped_type zip = nullptr;
				try
				{
					auto archive{ZipFactory::getInstance().get(fullPath)};
					auto item{archive->commonBase()};
					zip = item.length() ? (mapped_type) new Zip(fullPath, item) :
											(mapped_type) new Zip(fullPath);
				}
				catch (Exception e)
				{
				}
				return reportWrap(zip != nullptr ? zip : (mapped_type) new Symlink(fullPath));
			}
			if (is_symlink(status))
			{
				return reportWrap(new Symlink(fullPath));
			}
			return nullptr;
		}
		void destroy(const mapped_type& item) { delete item; }

	protected:
		const struct path& path;
	};

} // namespace ZipDirFs::Components

#endif // ZIPDIRFS_COMPONENTS_NATIVEFACTORY_H
