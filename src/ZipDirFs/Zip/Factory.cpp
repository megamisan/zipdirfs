/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Zip/Factory.h"
#include "ZipDirFs/Zip/Lib.h"
#include <boost/filesystem.hpp>
#include <mutex>

namespace ZipDirFs::Zip
{
	namespace
	{
		std::mutex zip_factory_get;
	} // namespace

	Factory::Factory() {}
	Factory::~Factory() {}
	Factory* Factory::instance = nullptr;
	std::map<boost::filesystem::path, std::weak_ptr<Archive>> Factory::archivesByPath;
	std::map<const Base::Lib*, std::weak_ptr<Archive>> Factory::archivesByData;

	Factory& Factory::getInstance() { return *instance; }

	std::shared_ptr<Archive> Factory::get(const boost::filesystem::path& p)
	{
		std::shared_ptr<Archive> result;
		std::lock_guard<std::mutex> guard(zip_factory_get);
		auto it = archivesByPath.find(p);
		if (it != archivesByPath.end() && it->second.expired())
		{
			archivesByPath.erase(it);
			it = archivesByPath.end();
		}
		if (it == archivesByPath.end())
		{
			auto data = Lib::open(p);
			result = std::shared_ptr<Archive>(new Archive(data),
				[this, data](Archive* a)
				{
					std::lock_guard<std::mutex> guard(zip_factory_get);
					auto it = this->archivesByData.find(data);
					if (it != this->archivesByData.end())
					{
						this->archivesByData.erase(it);
					}
					delete a;
				});
			archivesByPath.insert({p, result});
			archivesByData.insert({data, result});
		}
		else
		{
			result = it->second.lock();
		}
		return result;
	}

	std::shared_ptr<Archive> Factory::get(const Base::Lib* const& d) throw()
	{
		std::shared_ptr<Archive> result;
		std::lock_guard<std::mutex> guard(zip_factory_get);
		auto it = archivesByData.find(d);
		if (it != archivesByData.end() && !it->second.expired())
		{
			result = it->second.lock();
		}
		return result;
	}

	struct FactoryInstantiate
	{
		FactoryInstantiate() { Factory::instance = new Factory(); }
		~FactoryInstantiate() { delete Factory::instance; }
	} factoryInstanciator;

} // namespace ZipDirFs::Zip
