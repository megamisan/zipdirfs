/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIP_FACTORY_H
#define ZIPDIRFS_ZIP_FACTORY_H

#include "ZipDirFs/Zip/Archive.h"
#include "ZipDirFs/Zip/Base/Lib.h"
#include <map>
#include <memory>

namespace boost::filesystem
{
	struct path;
}

namespace ZipDirFs::Zip
{
	struct FactoryInstantiate;

	/**
	 * @brief A Factory to create or access zip archives
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class Factory
	{
	public:
		std::shared_ptr<Archive> get(const boost::filesystem::path&);
		std::shared_ptr<Archive> get(const Base::Lib* const&) throw();
		static Factory& getInstance();

	protected:
		Factory();
		~Factory();
		static Factory* instance;
		static std::map<boost::filesystem::path, std::weak_ptr<Archive>> archivesByPath;
		static std::map<const Base::Lib*, std::weak_ptr<Archive>> archivesByData;

	private:
		friend class FactoryInstantiate;
	};

} // namespace ZipDirFs::Zip

#endif // ZIPDIRFS_ZIP_FACTORY_H
