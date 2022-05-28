/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "Lib.h"
#include "ZipDirFs/Zip/Lib.h"
#include <boost/filesystem.hpp>

namespace Test::ZipDirFs::Zip
{
	namespace Fixtures
	{
		using LibBase = ::ZipDirFs::Zip::Lib;
		Lib::Lib()
		{
			LibBase::open = [this](const path& p) { return this->open(p); };
			LibBase::close = [this](LibP* const l) { return this->close(l); };
			LibBase::get_num_entries = [this](LibP* const l) { return this->get_num_entries(l); };
			LibBase::stat = [this](LibP* const l, const std::string& n)
			{ return this->stat(l, n); };
			LibBase::stat_index = [this](LibP* const l, const std::uint64_t i)
			{ return this->stat_index(l, i); };
			LibBase::get_name = [this](LibP* const l, std::uint64_t i)
			{ return this->get_name(l, i); };
			LibBase::fopen_index = [this](LibP* const l, std::uint64_t i)
			{ return this->fopen_index(l, i); };
			LibBase::fread = [this](LibP::File* const f, void* buf, std::uint64_t len)
			{ return this->fread(f, buf, len); };
			LibBase::fseek = [this](LibP::File* const f, int64_t const o, int const w)
			{ return this->fseek(f, o, w); };
			LibBase::ftell = [this](LibP::File* const f) { return this->ftell(f); };
			LibBase::fclose = [this](LibP::File* const f) { return this->fclose(f); };
		}
		Lib::~Lib() { LibBase::reset(); }

	} // namespace Fixtures
} // namespace Test::ZipDirFs::Zip
