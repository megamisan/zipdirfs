/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Components/ZipDirectoryEnumerator.h"
#include "ZipDirFs/Zip/Factory.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Components
{
	using namespace ZipDirFs::Zip;

	namespace
	{
		struct ZipDirEnumWrapper : public ZipDirFs::Containers::EntryIterator::Wrapper
		{
			ZipDirEnumWrapper(Archive::iterator&&);
			ZipDirEnumWrapper(const Archive::iterator&);
			~ZipDirEnumWrapper();

		protected:
			Wrapper* clone() const;
			reference dereference() const;
			void increment();
			bool equals(const Wrapper&) const;
			Archive::iterator it;
		};

		ZipDirFs::Containers::EntryIterator wrap(Archive::iterator&& it)
		{
			return ZipDirFs::Containers::EntryIterator(new ZipDirEnumWrapper(std::move(it)));
		}
	} // namespace
	ZipDirEnumWrapper::ZipDirEnumWrapper(Archive::iterator&& it) : it(std::move(it)) {}
	ZipDirEnumWrapper::ZipDirEnumWrapper(const Archive::iterator& it) : it(it) {}
	ZipDirEnumWrapper::~ZipDirEnumWrapper() {}
	ZipDirEnumWrapper::Wrapper* ZipDirEnumWrapper::clone() const
	{
		return new ZipDirEnumWrapper(it);
	}
	ZipDirEnumWrapper::reference ZipDirEnumWrapper::dereference() const { return *it; }
	void ZipDirEnumWrapper::increment() { ++it; }
	bool ZipDirEnumWrapper::equals(const ZipDirEnumWrapper::Wrapper& w) const
	{
		auto wp = dynamic_cast<const ZipDirEnumWrapper*>(&w);
		return wp != nullptr && it == wp->it;
	}

	ZipDirectoryEnumerator::ZipDirectoryEnumerator(
		const boost::filesystem::path& path, const std::string& item) :
		path(path),
		item(item), currentIt(wrap(Factory::getInstance().get(path)->begin(item))),
		endIt(wrap(Factory::getInstance().get(path)->end()))
	{
	}

	ZipDirectoryEnumerator::~ZipDirectoryEnumerator() {}

	void ZipDirectoryEnumerator::reset()
	{
		currentIt = wrap(Factory::getInstance().get(path)->begin(item));
	}
	void ZipDirectoryEnumerator::next() { ++currentIt; }
	bool ZipDirectoryEnumerator::valid() { return !(currentIt == endIt); }
	const std::string& ZipDirectoryEnumerator::current() { return *currentIt; }

} // namespace ZipDirFs::Components
