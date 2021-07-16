/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Zip/Archive.h"
#include "ZipDirFs/Zip/Factory.h"
#include "ZipDirFs/Zip/Lib.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Zip
{
	namespace
	{
		bool ArchiveIteratorMatch(const std::string& prefix, const std::string& name)
		{
			return (name.length() > prefix.length())
				&& (prefix.length() == 0 || name.substr(0, prefix.length()) == prefix)
				&& (name.find('/', prefix.length()) == std::string::npos);
		}
		bool ArchiveIteratorSkipIgnored(std::vector<std::string>::iterator& current,
			std::vector<std::string>::iterator&& end, const std::string& prefix)
		{
			while (current != end && !ArchiveIteratorMatch(prefix, *current))
				++current;
			return current != end;
		}
		std::string ArchiveIteratorExtractFilename(
			const std::string& name, const std::string& prefix)
		{
			return name.substr(prefix.length());
		}
	} // namespace
	Archive::Archive(Base::Lib* d) : data(d) {}
	Archive::~Archive() { Lib::close(data); }
	std::shared_ptr<Archive> Archive::create(Base::Lib* l)
	{
		auto p{std::shared_ptr<Archive>(new Archive(l))};
		p->weak_ptr = p;
		return p;
	}
	Archive::iterator Archive::begin()
	{
		const Base::Lib* const d = data;
		populate();
		return iterator(weak_ptr.lock());
	}
	Archive::iterator Archive::begin(const std::string& p)
	{
		const Base::Lib* const d = data;
		populate();
		return iterator(weak_ptr.lock(), p);
	}
	Archive::iterator Archive::end()
	{
		const Base::Lib* const d = data;
		return iterator(weak_ptr.lock(), names.end());
	}
	std::shared_ptr<Entry> Archive::open(const std::string& n)
	{
		const Base::Lib* const d = data;
		std::shared_ptr<Entry> result;
		populate();
		auto itI = nameAttributes.find(n);
		if (itI != nameAttributes.end())
		{
			std::lock_guard<std::mutex> lock(entries_mutex);
			auto itE = entries.find(std::get<0>(itI->second));
			if (itE != entries.end() && itE->second.expired())
			{
				entries.erase(itE);
				itE = entries.end();
			}
			if (itE == entries.end())
			{
				const Base::Lib* const d = data;
				result = std::shared_ptr<Entry>(
					new Entry(std::shared_ptr<Base::Lib>(weak_ptr.lock(), data), n,
						std::get<1>(itI->second)));
				entries.emplace(std::get<0>(itI->second), result);
			}
			else
			{
				result = itE->second.lock();
			}
		}
		return result;
	}
	void Archive::populate()
	{
		std::lock_guard<std::mutex> lock(names_mutex);
		if (!names.empty())
			return;
		std::uint64_t index = 0, count = Lib::get_num_entries(data), temporary = UINT64_MAX;
		std::vector<std::tuple<std::string, bool>> temp;
		names.reserve(count);
		for (; index < count; index++)
		{
			std::string name = Lib::get_name(data, index);
			temp.emplace_back(name, false);
			boost::filesystem::path p(name);
			while (p.has_parent_path())
			{
				std::string path = p.parent_path().native();
				temp.emplace_back(path, true);
				p = p.parent_path();
			}
			while (!temp.empty())
			{
				auto entry = temp.back();
				temp.pop_back();
				auto attrIt = nameAttributes.find(std::get<0>(entry));
				if (attrIt == nameAttributes.end())
				{
					names.push_back(std::get<0>(entry));
					nameAttributes.emplace(std::get<0>(entry),
						decltype(nameAttributes)::mapped_type{
							std::get<1>(entry) ? temporary-- : index, std::get<1>(entry)});
				}
				else if (std::get<1>(attrIt->second) == 0 && std::get<1>(entry))
				{
					attrIt->second =
						decltype(nameAttributes)::mapped_type{std::get<0>(attrIt->second), true};
				}
			}
		}
		names.shrink_to_fit();
	}
	Archive::ArchiveIterator::~ArchiveIterator() {}
	Archive::ArchiveIterator::reference Archive::ArchiveIterator::operator*() const
	{
		return filename;
	}
	Archive::ArchiveIterator& Archive::ArchiveIterator::operator++()
	{
		if (ArchiveIteratorSkipIgnored(++current, archive->names.end(), *prefix))
		{
			filename = ArchiveIteratorExtractFilename(*current, *prefix);
		}
		return *this;
	}
	bool Archive::ArchiveIterator::operator==(const Archive::ArchiveIterator& ai) const
	{
		return archive == ai.archive
			&& (ai.prefix == nullptr || prefix == nullptr || *prefix == *ai.prefix)
			&& current == ai.current;
	}
	void Archive::ArchiveIterator::swap(Archive::ArchiveIterator& ai)
	{
		std::swap(archive, ai.archive);
		std::swap(prefix, ai.prefix);
		std::swap(current, ai.current);
		std::swap(filename, ai.filename);
	}
	Archive::ArchiveIterator::ArchiveIterator(const std::shared_ptr<Archive>& a) :
		ArchiveIterator(a, "")
	{
	}
	Archive::ArchiveIterator::ArchiveIterator(
		const std::shared_ptr<Archive>& a, std::vector<std::string>::iterator&& it) :
		archive(a),
		prefix(std::shared_ptr<std::string>(nullptr)), current(std::move(it))
	{
	}
	Archive::ArchiveIterator::ArchiveIterator(
		const std::shared_ptr<Archive>& a, const std::string& p) :
		archive(a),
		prefix(std::shared_ptr<std::string>(new std::string(p))), current(a->names.begin())
	{
		if (ArchiveIteratorSkipIgnored(current, a->names.end(), *prefix))
		{
			filename = ArchiveIteratorExtractFilename(*current, *prefix);
		}
	}
} // namespace ZipDirFs::Zip

namespace std
{
	void swap(Archive::ArchiveIterator& it1, Archive::ArchiveIterator& it2) { it1.swap(it2); }
} // namespace std
