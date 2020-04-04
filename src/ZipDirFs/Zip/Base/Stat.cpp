/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Zip/Base/Stat.h"

namespace ZipDirFs::Zip::Base
{
	Stat::Stat(const std::uint64_t i, const std::string& n, const std::uint64_t s,
		const std::time_t m, const bool c) :
		index(i),
		name(n), size(s), changed(m), compressed(c)
	{
	}
	const std::uint64_t Stat::getIndex() const { return index; }
	const std::string& Stat::getName() const { return name; }
	const std::uint64_t Stat::getSize() const { return size; }
	const std::time_t Stat::getChanged() const { return changed; }
	const bool Stat::getCompressed() const { return compressed; }
} // namespace ZipDirFs::Zip::Base
