/*
 * Copyright © 2019-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_CONTAINERS_ENTRYITERATOR_H
#define ZIPDIRFS_CONTAINERS_ENTRYITERATOR_H

#include <iterator>
#include <memory>
#include <string>

namespace ZipDirFs::Containers
{
	/**
	 * @brief An iterator definition to work with non templated @link ZipDirFs::Containers::EntryList entry list @endlink
	 * @remarks Uses instances of the inner class Wrapper to do the real work.
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class EntryIterator : public std::iterator<std::forward_iterator_tag, const std::string, size_t>
	{
	public:
		/**
		 * An abstract providing the expected contract for iterating.
		 */
		class Wrapper
		{
		public:
			typedef EntryIterator::reference reference;
			virtual ~Wrapper();

		protected:
			virtual Wrapper* clone() const = 0;
			virtual reference dereference() const = 0;
			virtual void increment() = 0;
			virtual bool equals(const Wrapper&) const = 0;
			friend class EntryIterator;
			friend bool operator==(const Wrapper&, const Wrapper&);
		};
		typedef std::unique_ptr<Wrapper> pointer_wrapper;
		EntryIterator();
		EntryIterator(Wrapper*);
		EntryIterator(EntryIterator&&);
		EntryIterator(const EntryIterator&);
		EntryIterator& operator=(EntryIterator&&);
		EntryIterator& operator=(const EntryIterator&);
		~EntryIterator();
		reference operator*() const;
		EntryIterator& operator++();
		bool operator==(const EntryIterator&) const;
		void swap(EntryIterator&);
		const Wrapper* const getWrapper() const;

	protected:
		std::unique_ptr<Wrapper> wrapper;
	};

	bool operator==(const EntryIterator::Wrapper&, const EntryIterator::Wrapper&);
} // namespace ZipDirFs::Containers

namespace std
{
	using ZipDirFs::Containers::EntryIterator;
	void swap(EntryIterator& it1, EntryIterator& it2);

} // namespace std
#endif // ZIPDIRFS_CONTAINERS_ENTRYITERATOR_H
