/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Zip/Factory.h"
#include "StateReporter.h"
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

	Factory& Factory::getInstance() { return *instance; }

	std::shared_ptr<Archive> Factory::get(const boost::filesystem::path& p)
	{
		std::shared_ptr<Archive> result;
		StateReporter::Lock rl("ZIPF");
		rl.init();
		std::lock_guard<std::mutex> guard(zip_factory_get);
		rl.set();
		StateReporter::Log("zip-factory").stream << "Retrieving archive for " << p;
		auto it = archivesByPath.find(p);
		if (it != archivesByPath.end())
		{
			if (!it->second.expired())
			{
				StateReporter::Log("zip-factory").stream << "Existing archive found";
				result = it->second.lock();
				result == nullptr ? StateReporter::Log("zip-factory").stream
						<< "Existing archive instance expired while locking reference" :
									  StateReporter::Log("zip-factory").stream
						<< "Existing archive " << std::hex << (void*)result.get();
			}
			else
			{
				StateReporter::Log("zip-factory").stream << "Previous archive instance expired";
			}
			if (result == nullptr)
			{
				archivesByPath.erase(it);
				it = archivesByPath.end();
			}
		}
		if (it == archivesByPath.end())
		{
			auto data = Lib::open(p);
			reportArchiveOpened(p.native(), data);
			result = Archive::create(data);
			StateReporter::Log("zip-factory").stream << "Created archive " << std::hex
													 << (void*)result.get() << " with data "
													 << std::hex << (void*)data << " for " << p;
			archivesByPath.insert({p, result});
		}
		return result;
	}

	struct FactoryInstantiate
	{
		FactoryInstantiate() { Factory::instance = new Factory(); }
		~FactoryInstantiate() { delete Factory::instance; }
	} factoryInstanciator;

} // namespace ZipDirFs::Zip
