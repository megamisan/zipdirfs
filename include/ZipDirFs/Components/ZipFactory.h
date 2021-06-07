/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_COMPONENTS_ZIPFACTORY_H
#define ZIPDIRFS_COMPONENTS_ZIPFACTORY_H

#include "ZipDirFs/Containers/Helpers/Changed.h"
#include "ZipDirFs/Containers/Helpers/Factory.h"
#include "ZipDirFs/Zip/Factory.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Components
{
	using namespace ::boost::filesystem;
	using RealFactory = ::ZipDirFs::Zip::Factory;

	/**
	 * @brief A @link ZipDirFs::Containers::Helpers::Factory Factory @endlink for creating entries under a zip archive
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 * @tparam Directory Implementation class for virtual directory in archive.
	 * @tparam File Implementation class for file in archive.
	 */
	template <class Directory, class File>
	class ZipFactory : public ::ZipDirFs::Containers::Helpers::Factory
	{
	public:
		ZipFactory(const path& archive, const key_type& inner,
			std::shared_ptr<::ZipDirFs::Containers::Helpers::Changed>&& changed) :
			archive(archive),
			inner(inner), changed(changed)
		{
		}
		const mapped_type create(const key_type& name)
		{
			auto innerPath = inner + name;
			auto item = RealFactory::getInstance().get(archive)->open(innerPath);
			if (item != nullptr)
			{
				if (item->isDir())
				{
					return new Directory(archive, innerPath + "/", changed);
				}
				return new File(archive, innerPath, changed);
			}
			return nullptr;
		}
		void destroy(const mapped_type& item) { delete item; }

	protected:
		const path& archive;
		const key_type& inner;
		std::shared_ptr<::ZipDirFs::Containers::Helpers::Changed> changed;
	};

} // namespace ZipDirFs::Components

#endif // ZIPDIRFS_COMPONENTS_ZIPFACTORY_H
