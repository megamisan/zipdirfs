/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "helpers.h"

namespace std
{
	bool operator==(const string& it, const ::testing::internal::ReferenceWrapper<string>& wrap)
	{
		return it == (string&)wrap;
	}
	bool operator==(
		const string& it, const ::testing::internal::ReferenceWrapper<const string>& wrap)
	{
		return it == (const string&)wrap;
	}
} // namespace std
