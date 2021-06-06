/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_CONTAINERS_ENTRYGENERATOR_H
#define ZIPDIRFS_CONTAINERS_ENTRYGENERATOR_H

#include "ZipDirFs/Containers/EntryIterator.h"
#include "ZipDirFs/Containers/Helpers/Changed.h"
#include "ZipDirFs/Containers/Helpers/EntryListProxy.h"
#include "ZipDirFs/Containers/Helpers/Enumerator.h"
#include "ZipDirFs/Containers/Helpers/Factory.h"
#include "ZipDirFs/Utilities/MutexLocker.h"
#include <map>
#include <memory>
#include <set>
#include <string>

#ifdef BUILD_TEST
namespace Test::ZipDirFs::Containers::Fixtures
{
	class HolderStateInitializer;
}
#endif

namespace ZipDirFs::Containers
{
	/**
	 * A utility to maintain the entry lists.
	 *
	 * Using the different helpers, it helps iterating over the list
	 * entries and synchronizes it to the underlying source.
	 */
	class EntryGenerator
	{
	public:
		typedef Helpers::EntryListProxy proxy_type;
		typedef Helpers::Changed changed_type;
		typedef Helpers::Enumerator<std::string> enumerator_type;
		typedef Helpers::Factory factory_type;
		typedef Utilities::MutexLocker locker_type;
		typedef std::shared_ptr<proxy_type> proxy_ptr;
		typedef std::shared_ptr<changed_type> changed_ptr;
		typedef std::shared_ptr<enumerator_type> enumerator_ptr;
		typedef std::shared_ptr<factory_type> factory_ptr;
		typedef std::shared_ptr<locker_type> locker_ptr;
		EntryGenerator(proxy_ptr&&, changed_ptr&&, enumerator_ptr&&, factory_ptr&&, locker_ptr&&);
		virtual ~EntryGenerator();

		EntryIterator begin();
		EntryIterator end();
		EntryIterator remove(off_t);
		void add(EntryIterator&&, off_t);
		struct HolderBase
		{
			virtual ~HolderBase(){};
		};

	protected:
		std::shared_ptr<HolderBase> holder;

	private:
		/**
		 * An helper for internal use in unit tests.
		 * @internal
		 */
		struct HolderAccessor
		{
			HolderAccessor(const std::shared_ptr<HolderBase>& h);
			std::map<off_t, EntryIterator>& offsetWrappers() const;
			std::set<EntryIterator::Wrapper*>& allWrappers() const;
			EntryIterator atCombined(EntryIterator&&, EntryIterator&&) const;
			EntryIterator atDirect(EntryIterator&&) const;

		private:
			std::shared_ptr<HolderBase> holder;
		};
		HolderAccessor testAccessor() const;
#ifdef BUILD_TEST
		friend class ::Test::ZipDirFs::Containers::Fixtures::HolderStateInitializer;
#endif
	};
} // namespace ZipDirFs::Containers

#endif // ZIPDIRFS_CONTAINERS_ENTRYGENERATOR_H
