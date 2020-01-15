/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */

#include "EntryIteratorTestWrapper.h"

namespace Test::ZipDirFs::Containers
{
	namespace Fixtures
	{
			EntryIteratorTestWrapper::EntryIteratorTestWrapper(EntryIteratorTestWrapper::list_type::iterator v) : value(v) {}
			EntryIteratorTestWrapper::~EntryIteratorTestWrapper() {}
			EntryIteratorWrapperBase* EntryIteratorTestWrapper::clone() const {
				return new EntryIteratorTestWrapper(value);
			}
			EntryIteratorTestWrapper::reference EntryIteratorTestWrapper::dereference() const {
				return *value;
			}
			void EntryIteratorTestWrapper::increment() { ++value; }
			bool EntryIteratorTestWrapper::equals(const EntryIteratorWrapperBase& w) const {
				return dynamic_cast<const EntryIteratorTestWrapper*>(&w) != nullptr &&
					value == reinterpret_cast<const EntryIteratorTestWrapper*>(&w)->value;
			}
	}
} // namespace Test::ZipDirFs::Containers
