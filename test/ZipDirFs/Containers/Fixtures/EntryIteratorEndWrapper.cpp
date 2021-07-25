/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */

#include "EntryIteratorEndWrapper.h"

namespace Test::ZipDirFs::Containers
{
	namespace Fixtures
	{
		EntryIteratorEndWrapper::EntryIteratorEndWrapper(const std::int64_t v) : value(v) {}
		EntryIteratorEndWrapper::~EntryIteratorEndWrapper() {}
		EntryIteratorWrapperBase* EntryIteratorEndWrapper::clone() const
		{
			return new EntryIteratorEndWrapper(value);
		}
		EntryIteratorEndWrapper::reference EntryIteratorEndWrapper::dereference() const
		{
			return *reinterpret_cast<std::string*>(0);
		}
		void EntryIteratorEndWrapper::increment() {}
		bool EntryIteratorEndWrapper::equals(const EntryIteratorWrapperBase& w) const
		{
			return dynamic_cast<const EntryIteratorEndWrapper*>(&w) != nullptr
				&& value == reinterpret_cast<const EntryIteratorEndWrapper*>(&w)->value;
		}
		EntryIteratorEndWrapper::operator const std::int64_t() const { return value; }
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers
