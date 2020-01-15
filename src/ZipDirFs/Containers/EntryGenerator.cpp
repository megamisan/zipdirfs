/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Containers/EntryGenerator.h"
#include "fusekit/entry.h"

namespace ZipDirFs::Containers
{
	using proxy_ptr = EntryGenerator::proxy_ptr;
	using changed_ptr = EntryGenerator::changed_ptr;
	using enumerator_ptr = EntryGenerator::enumerator_ptr;
	using factory_ptr = EntryGenerator::factory_ptr;
	using WrapperBase = EntryIterator::Wrapper;
	using HolderBase = EntryGenerator::HolderBase;

	namespace
	{
		struct Syncer;
		struct SyncerAccessor;
		struct CombinedWrapper;
		struct DirectWrapper;

		struct Holder : public HolderBase
		{
			Holder(proxy_ptr&& p, changed_ptr&& c, enumerator_ptr&& e, factory_ptr&& f) :
				proxy(std::move(p)), changed(std::move(c)), enumerator(std::move(e)),
				factory(std::move(f)), syncer(nullptr)
			{
			}
			proxy_ptr proxy;
			changed_ptr changed;
			enumerator_ptr enumerator;
			factory_ptr factory;
			std::map<off_t, EntryIterator> offsetWrappers;
			std::set<WrapperBase*> allWrappers;
			std::unique_ptr<Syncer> syncer;
		};

		struct Syncer
		{
			static void create(Holder*);
			bool current(const EntryIterator&);
			bool advance();

		protected:
			Syncer(Holder*);
			Holder* holder;
			EntryIterator currentIt;

		private:
			bool generateCurrent();
			void removeEntry(const std::string&);
			friend class SyncerAccessor;
		};
		struct WrapperData
		{
			inline const EntryIterator& getIt() const { return currentIt; }

		protected:
			WrapperData(const std::shared_ptr<HolderBase>&, EntryIterator&&);
			WrapperData(const std::shared_ptr<HolderBase>&, const EntryIterator&);
			const WrapperBase& resolve(const WrapperBase&) const;
			std::shared_ptr<HolderBase> holderBase;
			Holder* const holder;
			EntryIterator currentIt;
		};
		struct CombinedWrapper : public WrapperBase, public WrapperData
		{
			CombinedWrapper(const std::shared_ptr<HolderBase>&, EntryIterator&&);
			~CombinedWrapper();

		protected:
			CombinedWrapper(const std::shared_ptr<HolderBase>&, const EntryIterator&);
			WrapperBase* clone() const;
			reference dereference() const;
			void increment();
			bool equals(const WrapperBase&) const;
		};
		struct DirectWrapper : public WrapperBase, public WrapperData
		{
			DirectWrapper(const std::shared_ptr<HolderBase>&, EntryIterator&&);
			~DirectWrapper();

		protected:
			DirectWrapper(const std::shared_ptr<HolderBase>&, const EntryIterator&);
			WrapperBase* clone() const;
			reference dereference() const;
			void increment();
			bool equals(const WrapperBase&) const;
		};
		struct SyncerAccessor
		{
			static std::unique_ptr<Syncer> create(Holder*, EntryIterator&& base);
		};
	} // namespace

	EntryGenerator::EntryGenerator(
		proxy_ptr&& p, changed_ptr&& c, enumerator_ptr&& e, factory_ptr&& f) :
		holder(new Holder(std::move(p), std::move(c), std::move(e), std::move(f)))
	{
	}

	EntryGenerator::~EntryGenerator() {}

	EntryIterator EntryGenerator::begin()
	{
		WrapperBase* result;
		Holder* base(reinterpret_cast<decltype(base)>(holder.get()));
		if (base->allWrappers.empty() && (*base->changed)())
		{
			Syncer::create(base);
		}
		if (base->syncer != nullptr)
		{
			result = new CombinedWrapper(holder, std::move(base->proxy->begin()));
		}
		else
		{
			result = new DirectWrapper(holder, std::move(base->proxy->begin()));
		}
		return EntryIterator(result);
	}
	EntryIterator EntryGenerator::end()
	{
		return reinterpret_cast<Holder*>(holder.get())->proxy->end();
	}
	EntryIterator EntryGenerator::remove(off_t offset)
	{
		Holder* base(reinterpret_cast<decltype(base)>(holder.get()));
		auto it = base->offsetWrappers.find(offset);
		if (it == base->offsetWrappers.end())
		{
			return end();
		}
		auto ei(std::move(it->second));
		base->offsetWrappers.erase(it);
		return ei;
	}
	void EntryGenerator::add(EntryIterator&& ei, off_t offset)
	{
		Holder* base(reinterpret_cast<decltype(base)>(holder.get()));
		base->offsetWrappers.insert({offset, std::move(ei)});
	}

	void Syncer::create(Holder* h)
	{
		h->syncer = std::unique_ptr<Syncer>(new Syncer(h));
		h->syncer->currentIt = h->proxy->begin();
		h->enumerator->reset();
		if (!h->syncer->generateCurrent())
		{
			h->syncer = nullptr;
		}
	}
	Syncer::Syncer(Holder* h) : holder(h) {}
	bool Syncer::current(const EntryIterator& it) { return it == currentIt; }
	bool Syncer::advance()
	{
		++currentIt;
		holder->enumerator->next();
		return generateCurrent();
	}
	bool Syncer::generateCurrent()
	{
		bool result = true;
		if (holder->enumerator->valid())
		{
			const std::string& key = holder->enumerator->current();
			bool keyDifferent = false;
			if ((currentIt == holder->proxy->end()) || // It cannot be both
				(keyDifferent = (key != *currentIt)))
			{
				fusekit::entry* entry = keyDifferent ? holder->proxy->find(key) : nullptr;
				if (entry != nullptr)
				{
					removeEntry(key);
				}
				else
				{
					entry = holder->factory->create(key);
				}
				currentIt = holder->proxy->insert(currentIt, key, entry);
			}
		}
		else
		{
			while (!(currentIt == holder->proxy->end()))
			{
				holder->factory->destroy(holder->proxy->find(*currentIt));
				currentIt = holder->proxy->erase(currentIt);
			}
			result = false;
		}
		return result;
	}
	void Syncer::removeEntry(const std::string& key)
	{
		EntryIterator it(currentIt);
		while (*it != key)
		{
			++it;
		}
		holder->proxy->erase(it);
	}
	WrapperData::WrapperData(const std::shared_ptr<HolderBase>& h, EntryIterator&& ei) :
		holderBase(h), holder(reinterpret_cast<decltype(holder)>(h.get())), currentIt(std::move(ei))
	{
	}
	WrapperData::WrapperData(const std::shared_ptr<HolderBase>& h, const EntryIterator& ei) :
		holderBase(h), holder(reinterpret_cast<decltype(holder)>(h.get())), currentIt(ei)
	{
	}
	const WrapperBase& WrapperData::resolve(const WrapperBase& w) const
	{
		const WrapperData* wd = dynamic_cast<const WrapperData*>(&w);
		if (wd != nullptr)
		{
			return *(wd->getIt().getWrapper());
		}
		return w;
	}
	CombinedWrapper::CombinedWrapper(const std::shared_ptr<HolderBase>& h, EntryIterator&& ei) :
		WrapperData(h, std::move(ei))
	{
		holder->allWrappers.insert(this);
	}
	CombinedWrapper::CombinedWrapper(
		const std::shared_ptr<HolderBase>& h, const EntryIterator& ei) :
		WrapperData(h, ei)
	{
		holder->allWrappers.insert(this);
	}
	CombinedWrapper::~CombinedWrapper() { holder->allWrappers.erase(this); }
	WrapperBase* CombinedWrapper::clone() const
	{
		return new CombinedWrapper(holderBase, currentIt);
	}
	CombinedWrapper::reference CombinedWrapper::dereference() const { return *currentIt; }
	void CombinedWrapper::increment()
	{
		if (holder->syncer != nullptr)
		{
			if (holder->syncer->current(currentIt))
			{
				if (!holder->syncer->advance())
				{
					holder->syncer = nullptr;
				}
			}
		}
		++currentIt;
	}
	bool CombinedWrapper::equals(const WrapperBase& w) const
	{
		return *(currentIt.getWrapper()) == resolve(w);
	}
	DirectWrapper::DirectWrapper(const std::shared_ptr<HolderBase>& h, EntryIterator&& ei) :
		WrapperData(h, std::move(ei))
	{
		holder->allWrappers.insert(this);
	}
	DirectWrapper::DirectWrapper(const std::shared_ptr<HolderBase>& h, const EntryIterator& ei) :
		WrapperData(h, ei)
	{
		holder->allWrappers.insert(this);
	}
	DirectWrapper::~DirectWrapper() { holder->allWrappers.erase(this); }
	WrapperBase* DirectWrapper::clone() const { return new DirectWrapper(holderBase, currentIt); }
	DirectWrapper::reference DirectWrapper::dereference() const { return *currentIt; }
	void DirectWrapper::increment() { ++currentIt; }
	bool DirectWrapper::equals(const WrapperBase& w) const
	{
		return *(currentIt.getWrapper()) == resolve(w);
	}
	EntryGenerator::HolderAccessor::HolderAccessor(
		const std::shared_ptr<EntryGenerator::HolderBase>& h) :
		holder(h)
	{
	}
	std::map<off_t, EntryIterator>& EntryGenerator::HolderAccessor::offsetWrappers() const
	{
		return static_cast<Holder*>(holder.get())->offsetWrappers;
	}
	std::set<WrapperBase*>& EntryGenerator::HolderAccessor::allWrappers() const
	{
		return static_cast<Holder*>(holder.get())->allWrappers;
	}
	EntryIterator EntryGenerator::HolderAccessor::atCombined(
		EntryIterator&& base, EntryIterator&& pos) const
	{
		Holder& holder = *static_cast<Holder*>(this->holder.get());
		holder.syncer = SyncerAccessor::create(&holder, std::move(base));
		return EntryIterator(new CombinedWrapper(this->holder, std::move(pos)));
	}
	EntryIterator EntryGenerator::HolderAccessor::atDirect(EntryIterator&& pos) const
	{
		Holder& holder = *static_cast<Holder*>(this->holder.get());
		holder.syncer = nullptr;
		return EntryIterator(new DirectWrapper(this->holder, std::move(pos)));
	}
	std::unique_ptr<Syncer> SyncerAccessor::create(Holder* h, EntryIterator&& base)
	{
		std::unique_ptr<Syncer> syncer(new Syncer(h));
		syncer->currentIt = std::move(base);
		return syncer;
	}
	EntryGenerator::HolderAccessor EntryGenerator::testAccessor() const
	{
		return HolderAccessor(holder);
	}
} // namespace ZipDirFs::Containers
