/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_ZIP_FIXTURES_SIMPLESPAN_H
#define TEST_ZIPDIRFS_ZIP_FIXTURES_SIMPLESPAN_H

#include <cstddef>
#include <type_traits>

namespace Test::ZipDirFs::Zip
{
	namespace Fixtures
	{
		struct SimpleSpanImpl
		{
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;

		protected:
			constexpr SimpleSpanImpl(void* d, size_type s) : _data(d), _size(s){};
			~SimpleSpanImpl() noexcept;
			size_type size() const;
			void set(void*, size_type);
			struct Content
			{
				constexpr Content(const SimpleSpanImpl&);

			protected:
				const SimpleSpanImpl& ref;
				friend bool operator==(const Content&, const Content&);
			};
			Content content() const;

		private:
			void* _data;
			size_type _size;
			friend bool operator==(const Content&, const Content&);
		};

		template <typename T, SimpleSpanImpl::size_type Size>
		struct SimpleSpan : public SimpleSpanImpl
		{
			using element_type = T;
			using value_type = std::remove_cv_t<element_type>;
			using pointer = element_type*;
			using const_pointer = const element_type*;
			using reference = element_type&;
			using const_reference = const element_type&;
			using iterator = pointer;
			static constexpr size_type extent = Size;
			static constexpr size_type baseExtent = extent * sizeof(element_type);
			constexpr SimpleSpan() noexcept : SimpleSpanImpl(nullptr, baseExtent) {}
			template <size_type N>
			constexpr SimpleSpan(element_type (&arr)[N]) noexcept : SimpleSpanImpl(arr, baseExtent)
			{
				static_assert(N == extent);
			}
			template <typename It>
			constexpr SimpleSpan(It first) : SimpleSpanImpl(&*first, baseExtent)
			{
			}
			constexpr size_type size() { return extent; }
			Content content() const { return SimpleSpanImpl::content(); };
		};

		constexpr SimpleSpanImpl::size_type dynamic_extend = -1;

		template <typename T>
		struct SimpleSpan<T, dynamic_extend> : public SimpleSpanImpl
		{
			using element_type = T;
			using value_type = std::remove_cv_t<element_type>;
			using pointer = element_type*;
			using const_pointer = const element_type*;
			using reference = element_type&;
			using const_reference = const element_type&;
			using iterator = pointer;
			constexpr SimpleSpan() noexcept : SimpleSpanImpl(nullptr, 0) {}
			template <size_type N>
			constexpr SimpleSpan(element_type (&arr)[N]) noexcept :
				SimpleSpanImpl(arr, N * sizeof(element_type))
			{
			}
			template <typename It>
			constexpr SimpleSpan(It first, size_type count) :
				SimpleSpanImpl(&*first, count * sizeof(element_type))
			{
			}
			constexpr size_type size() { return SimpleSpanImpl::size() / sizeof(element_type); }
			Content content() const { return SimpleSpanImpl::content(); };
		};

		bool operator==(const SimpleSpanImpl::Content&, const SimpleSpanImpl::Content&);
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Zip

#endif // TEST_ZIPDIRFS_ZIP_FIXTURES_SIMPLESPAN_H
