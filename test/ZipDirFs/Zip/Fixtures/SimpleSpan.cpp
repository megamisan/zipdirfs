/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "SimpleSpan.h"
#include <cstring>

namespace Test::ZipDirFs::Zip
{
	namespace Fixtures
	{
		SimpleSpanImpl::~SimpleSpanImpl() {}
		SimpleSpanImpl::size_type SimpleSpanImpl::size() const { return _size; };
		void SimpleSpanImpl::set(void* d, size_type s) { _data = d, _size = s; }
		SimpleSpanImpl::Content SimpleSpanImpl::content() const { return Content(*this); }
		constexpr SimpleSpanImpl::Content::Content(const SimpleSpanImpl& r) : ref(r) {}
		bool operator==(const SimpleSpanImpl::Content& c1, const SimpleSpanImpl::Content& c2)
		{
			return c1.ref._size == c2.ref._size
				&& std::memcmp(c1.ref._data, c2.ref._data, c1.ref._size) == 0;
		}
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Zip
