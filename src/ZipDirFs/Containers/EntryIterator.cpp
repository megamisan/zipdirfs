/*
 * Copyright © 2019-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Containers/EntryIterator.h"
#include <utility>

namespace ZipDirFs::Containers
{
	EntryIterator::EntryIterator() : wrapper() {}
	EntryIterator::EntryIterator(EntryIterator::Wrapper* w) : wrapper(w) {}
	EntryIterator::EntryIterator(EntryIterator&& it) : wrapper(::std::move(it.wrapper)) {}
	EntryIterator::EntryIterator(const EntryIterator& it) : wrapper(it.wrapper->clone()) {}
	EntryIterator& EntryIterator::operator=(EntryIterator&& it)
	{
		wrapper = std::move(it.wrapper);
		return *this;
	}
	EntryIterator& EntryIterator::operator=(const EntryIterator& it)
	{
		wrapper =
			EntryIterator::pointer_wrapper(it.wrapper == nullptr ? nullptr : it.wrapper->clone());
		return *this;
	}
	EntryIterator::~EntryIterator() {}
	EntryIterator::reference EntryIterator::operator*() const { return wrapper->dereference(); }
	EntryIterator& EntryIterator::operator++()
	{
		wrapper->increment();
		return *this;
	}
	bool EntryIterator::operator==(const EntryIterator& it) const
	{
		return (wrapper == nullptr) ?
			  ((it.wrapper == nullptr) ? true : false) :
			  ((it.wrapper == nullptr) ? false : wrapper->equals(*it.wrapper));
	}
	void EntryIterator::swap(EntryIterator& it) { wrapper.swap(it.wrapper); }
	EntryIterator::Wrapper::~Wrapper() {}

	const EntryIterator::Wrapper* const EntryIterator::getWrapper() const { return wrapper.get(); }

	bool operator==(const EntryIterator::Wrapper& w1, const EntryIterator::Wrapper& w2)
	{
		return w1.equals(w2);
	}
} // namespace ZipDirFs::Containers

namespace std
{
	using ZipDirFs::Containers::EntryIterator;
	void swap(EntryIterator& it1, EntryIterator& it2) { it1.swap(it2); }

} // namespace std
