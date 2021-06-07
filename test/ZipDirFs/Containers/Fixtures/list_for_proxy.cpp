/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "list_for_proxy.h"

namespace Test::ZipDirFs::Containers
{
	namespace Fixtures
	{
		list_for_proxy_iterator_equals_helper* list_for_proxy_iterator_equals_helper::instance;
		list_for_proxy_iterator_equals_helper::list_for_proxy_iterator_equals_helper()
		{
			instance = this;
		}
		list_for_proxy_iterator_equals_helper::~list_for_proxy_iterator_equals_helper()
		{
			instance = nullptr;
		}

	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers
