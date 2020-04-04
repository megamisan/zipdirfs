/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_UTILITIES_TYPE_TRAITS_H_INCLUDED
#define ZIPDIRFS_UTILITIES_TYPE_TRAITS_H_INCLUDED

#include <boost/filesystem.hpp>

namespace ZipDirFs::Utilities
{
	template <typename _Tp>
	class __has_root_path_impl_helper
	{
		template <typename _Tp,
			typename = decltype(
				typeof(boost::filesystem::path) == typeof(declval<_Tp>().getRootPath()))>
		static std::true_type __test(int);

		template <typename, typename>
		static std::false_type __test(...);

	public:
		typedef decltype(__test<_Tp>(0)) type;
	};

	template <typename _Tp, typename _Up>
	struct has_root_path_impl : public __has_root_path_impl_helper<_Tp, _Up>::type
	{
	};

	template <typename _Tp, bool = std::__is_referenceable<_Tp>::value>
	struct __has_root_path_impl;

	template <typename _Tp>
	struct __has_root_path_impl<_Tp, false> : public std::false_type
	{
	};

	template <typename _Tp>
	struct __has_root_path_impl<_Tp, true> : public has_root_path_impl<_Tp>
	{
	};

	template <typename _Tp>
	struct has_root_path : public __has_root_path_impl<_Tp>
	{
	};

	// L1295

} // namespace ZipDirFs::Utilities

#endif // ZIPDIRFS_UTILITIES_TYPE_TRAITS_H_INCLUDED
