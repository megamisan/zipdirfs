/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Components/ZipDirectoryEnumerator.h"
#include "ZipDirFs/Zip/Factory.h"
#include <boost/filesystem.hpp>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <set>
#include <thread>
#include <tuple>

namespace ZipDirFs::Components
{
	using namespace ZipDirFs::Zip;

	namespace
	{
		struct Holder : public ZipDirectoryEnumerator::HolderBase
		{
			Holder(Archive::iterator&& it, Archive::iterator&&);
			Archive::iterator currentIt, endIt;
		};

		struct less;

		struct Cleaner
		{
			typedef std::tuple<std::time_t, std::unique_ptr<Holder>, ZipDirectoryEnumerator*>
				value_type;
			typedef std::unique_lock<std::mutex> lock_type;
			Cleaner(const std::time_t);
			~Cleaner();
			static void CollectFor(ZipDirectoryEnumerator*);
			static void Add(std::unique_ptr<Holder>&& holder, ZipDirectoryEnumerator*);

		private:
			struct less
			{
				bool operator()(const Cleaner::value_type&, const Cleaner::value_type&) const;
			};
			const std::time_t waitTime;
			std::condition_variable wakeup;
			std::set<value_type, less> cleanQueue;
			std::thread thread;
			bool stop;
			bool stopWhenEmpty;
			void doWork();
			void CollectExpired();
			static std::tuple<lock_type, std::shared_ptr<Cleaner>> GetCleaner(bool);
			static void ReleaseCleaner(lock_type lock);
			static std::shared_ptr<Cleaner> cleaner;
			static std::mutex access;
		};
		std::shared_ptr<Cleaner> Cleaner::cleaner;
		std::mutex Cleaner::access;
	} // namespace

	extern std::time_t zipDirectoryEnumeratorDelay;
	std::time_t zipDirectoryEnumeratorDelay = 60;

	Holder::Holder(Archive::iterator&& currentIt, Archive::iterator&& endIt) :
		currentIt(currentIt), endIt(endIt)
	{
	}

	Cleaner::Cleaner(const std::time_t waitTime) :
		waitTime(waitTime), stop(false), stopWhenEmpty(false),
		thread(std::bind(std::mem_fn(&Cleaner::doWork), this))
	{
	}
	Cleaner::~Cleaner()
	{
		{
			lock_type lock(access);
			stop = true;
			wakeup.notify_all();
		}
		thread.join();
	}
	void Cleaner::CollectFor(ZipDirectoryEnumerator* parent)
	{
		auto cleanerLock = Cleaner::GetCleaner(false);
		if (std::get<1>(cleanerLock) == nullptr)
		{
			return;
		}
		auto& cleaner = *std::get<1>(cleanerLock);
		auto it = cleaner.cleanQueue.begin(), endIt = cleaner.cleanQueue.end();
		while (it != endIt)
		{
			if (std::get<2>(*it) == parent)
			{
				it = cleaner.cleanQueue.erase(it);
			}
			else
			{
				++it;
			}
		}
		cleaner.stopWhenEmpty = true;
		cleaner.wakeup.notify_all();
	}
	void Cleaner::Add(std::unique_ptr<Holder>&& holder, ZipDirectoryEnumerator* parent)
	{
		if (holder != nullptr)
		{
			auto cleanerLock = Cleaner::GetCleaner(true);
			auto& cleaner = *std::get<1>(cleanerLock);
			cleaner.cleanQueue.emplace(
				std::time(NULL) + cleaner.waitTime, std::move(holder), parent);
			cleaner.wakeup.notify_all();
		}
	}
	bool Cleaner::less::operator()(
		const Cleaner::value_type& lhs, const Cleaner::value_type& rhs) const
	{
		auto first = std::get<0>(lhs) - std::get<0>(rhs);
		return first != 0 ? (first < 0) : (std::get<1>(lhs) < std::get<1>(rhs));
	}
	void Cleaner::doWork()
	{
		lock_type lock(access);
		std::time_t lastTime = std::time(nullptr);
		while (!stop)
		{
			if (cleanQueue.empty())
			{
				if (!stopWhenEmpty)
				{
					wakeup.wait(lock);
				}
				else
				{
					stopWhenEmpty = false;
					std::thread(Cleaner::ReleaseCleaner, std::move(lock)).detach();
					lock = lock_type(access);
				}
			}
			else
			{
				std::time_t now = std::time(nullptr);
				std::time_t top = std::get<0>(*cleanQueue.begin());
				if (top <= now
					|| (now == lastTime ?
								 wakeup.wait_for(lock, std::chrono::milliseconds(500)) :
								 wakeup.wait_until(lock, std::chrono::system_clock::from_time_t(top)))
						== std::cv_status::timeout)
				{
					CollectExpired();
				}
				lastTime = now;
			}
		}
	}
	void Cleaner::CollectExpired()
	{
		auto it = cleanQueue.begin(), endIt = cleanQueue.end();
		auto now = std::time(NULL);
		while (it != endIt && std::get<0>(*it) <= now)
		{
			it = cleanQueue.erase(it);
		}
	}
	std::tuple<Cleaner::lock_type, std::shared_ptr<Cleaner>> Cleaner::GetCleaner(bool create)
	{
		lock_type lock(access);
		if (cleaner == nullptr && create)
		{
			cleaner = std::shared_ptr<Cleaner>(new Cleaner(zipDirectoryEnumeratorDelay));
		}
		return std::make_tuple(std::move(lock), cleaner);
	}
	void Cleaner::ReleaseCleaner(lock_type lock)
	{
		auto old = std::move(cleaner);
		lock.unlock();
		old = nullptr;
	}

	ZipDirectoryEnumerator::ZipDirectoryEnumerator(
		const boost::filesystem::path& path, const std::string& item) :
		path(path),
		item(item), holder(std::unique_ptr<HolderBase>(std::unique_ptr<Holder>(nullptr))),
		atEnd(false)
	{
	}

	ZipDirectoryEnumerator::~ZipDirectoryEnumerator() { Cleaner::CollectFor(this); }

	void ZipDirectoryEnumerator::reset()
	{
		auto archive = Factory::getInstance().get(path);
		auto instance = new Holder(archive->begin(item), archive->end());
		Cleaner::Add(std::unique_ptr<Holder>(dynamic_cast<Holder*>(holder.release())), this);
		holder = std::unique_ptr<HolderBase>(std::unique_ptr<Holder>(instance));
		if (instance->currentIt == instance->endIt)
		{
			Cleaner::Add(std::unique_ptr<Holder>(dynamic_cast<Holder*>(holder.release())), this);
			atEnd = true;
		}
	}
	void ZipDirectoryEnumerator::next()
	{
		if (holder == nullptr && !atEnd)
		{
			reset();
		}
		if (holder != nullptr)
		{
			auto instance = dynamic_cast<Holder*>(holder.get());
			++instance->currentIt;
			if (instance->currentIt == instance->endIt)
			{
				Cleaner::Add(
					std::unique_ptr<Holder>(dynamic_cast<Holder*>(holder.release())), this);
				atEnd = true;
			}
		}
	}
	bool ZipDirectoryEnumerator::valid()
	{
		if (holder == nullptr && !atEnd)
		{
			reset();
		}
		return !atEnd;
	}
	const std::string& ZipDirectoryEnumerator::current()
	{
		if (holder == nullptr && !atEnd)
		{
			reset();
		}
		return *dynamic_cast<Holder*>(holder.get())->currentIt;
	}
} // namespace ZipDirFs::Components
