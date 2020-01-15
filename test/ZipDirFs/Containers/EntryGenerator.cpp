/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#define BUILD_TEST
#include "ZipDirFs/Containers/EntryGenerator.h"
#include "EntryGenerator.h"
#include "Fixtures/EntryIteratorEndWrapper.h"
#include "Fixtures/EntryIteratorWrapperMock.h"
#include "Fixtures/HolderStateInitializer.h"
#include "Fixtures/helpers.h"
#include "test/gtest.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Containers
{
	using Fixtures::EntryIteratorEndWrapper;
	using Fixtures::EntryIteratorWrapperMock;
	using ::testing::ByRef;
	using ::testing::Eq;
	using ::testing::Return;
	using ::testing::ReturnRef;
	using ::ZipDirFs::Containers::EntryGenerator;

#define ENTRY_GENERATOR(var, proxy, changed, enumerator, factory) \
	EntryGenerator var( \
		std::move(EntryGenerator::proxy_ptr(proxy)), \
		std::move(EntryGenerator::changed_ptr(changed)), \
		std::move(EntryGenerator::enumerator_ptr(enumerator)), \
		std::move(EntryGenerator::factory_ptr(factory)))

	bool WrapperAccess::invokeEquals(const EntryIterator::Wrapper& w1, const EntryIterator::Wrapper& w2)
	{
		return reinterpret_cast<const WrapperAccess*>(&w1)->equals(w2);
	}

	void EntryGeneratorTest::makeMocks()
	{
		changed = new ChangedMock;
		elp = new EntryListProxyMock;
		enumerator = new EnumeratorMock;
		factory = new FactoryMock;
	}

	TEST_F(EntryGeneratorTest, InitializeAndFree)
	{
		ChangedMockDestructHelper dc;
		EntryListProxyMockDestructHelper delp;
		EnumeratorMockDestructHelper de;
		FactoryMockDestructHelper df;
		makeMocks();
		EXPECT_CALL(dc, destroy(changed));
		EXPECT_CALL(delp, destroy(elp));
		EXPECT_CALL(de, destroy(enumerator));
		EXPECT_CALL(df, destroy(factory));
		ENTRY_GENERATOR(, elp, changed, enumerator, factory);
	}

	ACTION_P(ReturnIterator, builder) { return EntryIterator(builder()); }
	ACTION_P2(ReturnIterator, output, builder)
	{
		builder(output);
		return EntryIterator(*output);
	}
	ACTION_P2(ReturnWrapper, output, creator) { return creator(output); }
	ACTION_P3(ReturnWrapper, output, creator, sourceId)
	{
		auto result = creator(output);
		result->source = sourceId;
		std::cerr << "Wrapper cloned from { source = " << sourceId << " }" << std::endl;
		return result;
	}
	MATCHER_P(IsEnd, value, "Match the value in a EntryIteratorEndWrapper.")
	{
		return dynamic_cast<const EntryIteratorEndWrapper*>(arg) != nullptr
			&& value == *(reinterpret_cast<const EntryIteratorEndWrapper*>(arg));
	}
	MATCHER_P(HaveWrapper, wrapper, "Match the wrapper in EntryIterator.")
	{
		return arg.getWrapper() == wrapper;
	}

	TEST_F(EntryGeneratorTest, beginEmptyNotChanged)
	{
		EntryIteratorWrapperMock* wrapperMock;
		makeMocks();
		EXPECT_CALL(*changed, invoke()).WillOnce(Return(false));
		EXPECT_CALL(*elp, begin())
			.WillOnce(ReturnIterator(&wrapperMock, [](EntryIteratorWrapperMock** output) {
				return *output = new ::testing::StrictMock<EntryIteratorWrapperMock>;
			}));
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		auto it = eg.begin();
		EXPECT_CALL(*wrapperMock, equals_proxy(wrapperMock)).WillOnce(Return(true));
		EXPECT_NE(wrapperMock, it.getWrapper());
		EXPECT_PRED_FORMAT2([](const char* nameA, const char* nameB, const EntryIterator::Wrapper& a, const EntryIterator::Wrapper& b) {
			if (WrapperAccess::invokeEquals(a, b)) return ::testing::AssertionSuccess();
			else return ::testing::internal::CmpHelperEQFailure(nameA, nameB, a, b);
		}, *it.getWrapper(), *static_cast<EntryIterator::Wrapper*>(wrapperMock));
		EXPECT_EQ(state.allWrappers().size(), 1);
	}

	TEST_F(EntryGeneratorTest, beginEmptyChanged)
	{
		EntryIteratorWrapperMock* wrapperMock;
		const std::int64_t endValue = ::Test::rand(UINT32_MAX);
		makeMocks();
		EXPECT_CALL(*changed, invoke()).WillOnce(Return(true));
		EXPECT_CALL(*enumerator, reset());
		EXPECT_CALL(*enumerator, valid()).WillOnce(Return(false));
		EXPECT_CALL(*elp, begin())
			.Times(2)
			.WillRepeatedly(
				ReturnIterator(&wrapperMock, [endValue](EntryIteratorWrapperMock** output) {
					*output = new ::testing::StrictMock<EntryIteratorWrapperMock>;
					EXPECT_CALL(**output, equals_proxy(IsEnd(endValue))).WillOnce(Return(true));
					return *output;
				}));
		EXPECT_CALL(*elp, end()).WillOnce(ReturnIterator([endValue]() {
			return new EntryIteratorEndWrapper(endValue);
		}));
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		auto it = eg.begin();
		EXPECT_NE(wrapperMock, it.getWrapper());
		EXPECT_EQ(it, ::ZipDirFs::Containers::EntryIterator(new EntryIteratorEndWrapper(endValue)));
		EXPECT_EQ(state.allWrappers().size(), 1);
	}

	TEST_F(EntryGeneratorTest, end)
	{
		EntryIteratorEndWrapper* endWrapper;
		const std::int64_t endValue = ::Test::rand(UINT32_MAX);
		makeMocks();
		EXPECT_CALL(*elp, end())
			.WillOnce(ReturnIterator(&endWrapper, [endValue](EntryIteratorEndWrapper** output) {
				return *output = new EntryIteratorEndWrapper(endValue);
			}));
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		auto it = eg.end();
		EXPECT_EQ(endWrapper, it.getWrapper());
	}

	TEST_F(EntryGeneratorTest, beginNotEmptyNotChanged)
	{
		EntryIteratorWrapperMock *wrapperMock, *wrapperMock2;
		auto buildWrapper = [](EntryIteratorWrapperMock** output) {
			return *output = new ::testing::StrictMock<EntryIteratorWrapperMock>;
		};
		makeMocks();
		EXPECT_CALL(*changed, invoke()).WillOnce(Return(false));
		EXPECT_CALL(*elp, begin())
			.WillOnce(ReturnIterator(&wrapperMock, buildWrapper))
			.WillOnce(ReturnIterator(&wrapperMock2, buildWrapper))
			.RetiresOnSaturation();
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		auto it = eg.begin();
		auto it2 = eg.begin();
		EXPECT_CALL(*wrapperMock, equals_proxy(wrapperMock2)).WillOnce(Return(true));
		EXPECT_EQ(typeid(it), typeid(it2));
		EXPECT_EQ(it, it2);
		EXPECT_EQ(state.allWrappers().size(), 2);
	}

	TEST_F(EntryGeneratorTest, beginNotEmptyChangedSame)
	{
		EntryIteratorWrapperMock *wrapperMock, *wrapperMock2;
		const std::int64_t endValue = ::Test::rand(UINT32_MAX);
		const std::string firstValue = std::string("first") + std::to_string(::Test::rand(UINT32_MAX));
		auto buildWrapper1 = [firstValue, endValue](EntryIteratorWrapperMock** output) {
			*output = new ::testing::StrictMock<EntryIteratorWrapperMock>;
			EXPECT_CALL(**output, dereference()).WillOnce(ReturnRef(firstValue));
			EXPECT_CALL(**output, equals_proxy(IsEnd(endValue))).WillOnce(Return(false));
			return *output;
		};
		auto buildWrapper2 = [](EntryIteratorWrapperMock** output) {
			return *output = new ::testing::StrictMock<EntryIteratorWrapperMock>;
		};
		makeMocks();
		EXPECT_CALL(*changed, invoke()).WillOnce(Return(true));
		EXPECT_CALL(*enumerator, reset());
		EXPECT_CALL(*enumerator, valid()).WillOnce(Return(true));
		EXPECT_CALL(*enumerator, current()).WillOnce(ReturnRef(firstValue));
		EXPECT_CALL(*elp, begin())
			.WillOnce(ReturnIterator(&wrapperMock, buildWrapper1))
			.WillOnce(ReturnIterator(&wrapperMock, buildWrapper2))
			.WillOnce(ReturnIterator(&wrapperMock2, buildWrapper2))
			.RetiresOnSaturation();
		EXPECT_CALL(*elp, end()).WillOnce(ReturnIterator([endValue]() {
			return new EntryIteratorEndWrapper(endValue);
		}));
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		auto it = eg.begin();
		auto it2 = eg.begin();
		EXPECT_CALL(*wrapperMock, equals_proxy(wrapperMock2)).WillOnce(Return(true));
		EXPECT_EQ(typeid(it), typeid(it2));
		EXPECT_EQ(it, it2);
		EXPECT_EQ(state.allWrappers().size(), 2);
	}

	TEST_F(EntryGeneratorTest, add)
	{
		EntryIteratorWrapperMock* wrapperMock = new EntryIteratorWrapperMock;
		const std::int64_t offsetValue = ::Test::rand(UINT32_MAX);
		makeMocks();
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		eg.add(std::move(EntryIterator(wrapperMock)), offsetValue);
		ASSERT_EQ(state.offsetWrappers().size(), 1);
		EXPECT_EQ(state.offsetWrappers().begin()->first, offsetValue);
		EXPECT_EQ(state.offsetWrappers().begin()->second.getWrapper(), wrapperMock);
	}

	TEST_F(EntryGeneratorTest, remove)
	{
		EntryIteratorWrapperMock* wrapperMock = new EntryIteratorWrapperMock;
		const std::int64_t offsetValue = ::Test::rand(UINT32_MAX);
		makeMocks();
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		state.offsetWrappers().insert({offsetValue, EntryIterator(wrapperMock)});
		auto it = eg.remove(offsetValue);
		EXPECT_EQ(it.getWrapper(), wrapperMock);
	}

	TEST_F(EntryGeneratorTest, removeMissing)
	{
		EntryIteratorEndWrapper* endWrapper;
		const std::int64_t offsetValue = ::Test::rand(UINT32_MAX);
		const std::int64_t endValue = ::Test::rand(UINT32_MAX);
		makeMocks();
		EXPECT_CALL(*elp, end())
			.WillOnce(ReturnIterator(&endWrapper, [endValue](EntryIteratorEndWrapper** output) {
				return *output = new EntryIteratorEndWrapper(endValue);
			}));
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		auto it = eg.remove(offsetValue);
		EXPECT_EQ(it.getWrapper(), endWrapper);
	}

	TEST_F(EntryGeneratorTest, itIncDirect)
	{
		EntryIteratorWrapperMock* wrapperMock = new EntryIteratorWrapperMock;
		makeMocks();
		EXPECT_CALL(*wrapperMock, increment());
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		EntryIterator direct(state.atDirect(EntryIterator(wrapperMock)));
		++direct;
	}

	TEST_F(EntryGeneratorTest, itIncCombinedNoChange)
	{
		::testing::InSequence seq;
		const std::int64_t endValue = ::Test::rand(UINT32_MAX);
		const std::string nextValue = std::string("next") + std::to_string(::Test::rand(UINT32_MAX));
		EntryIteratorWrapperMock* wrapperMockBase = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockPos = new EntryIteratorWrapperMock;
		makeMocks();
		EXPECT_CALL(*wrapperMockPos, equals_proxy(wrapperMockBase)).WillOnce(Return(true));
		EXPECT_CALL(*wrapperMockBase, increment());
		EXPECT_CALL(*enumerator, next());
		EXPECT_CALL(*enumerator, valid()).WillOnce(Return(true));
		EXPECT_CALL(*enumerator, current()).WillOnce(ReturnRef(nextValue));
		EXPECT_CALL(*elp, end()).WillOnce(ReturnIterator([endValue]() {
			return new EntryIteratorEndWrapper(endValue);
		}));
		EXPECT_CALL(*wrapperMockBase, equals_proxy(IsEnd(endValue))).WillOnce(Return(false));
		EXPECT_CALL(*wrapperMockBase, dereference()).WillOnce(ReturnRef(nextValue));
		EXPECT_CALL(*wrapperMockPos, increment());
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		EntryIterator combined(
			state.atCombined(EntryIterator(wrapperMockBase), EntryIterator(wrapperMockPos)));
		++combined;
	}

	TEST_F(EntryGeneratorTest, itIncCombinedChangeKeyExists)
	{
		::testing::InSequence seq;
		const std::int64_t endValue = ::Test::rand(UINT32_MAX);
		fusekit::entry* const foundEntry = reinterpret_cast<decltype(foundEntry)>(::Test::rand(UINT32_MAX));
		const std::string nextValue = std::string("next") + std::to_string(::Test::rand(UINT32_MAX));
		const std::string otherValue = std::string("other") + std::to_string(::Test::rand(UINT32_MAX));
		EntryIteratorWrapperMock* wrapperMockBase = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockBase2 = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockPos = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockClone = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockClone2 = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockClone3 = new EntryIteratorWrapperMock;
		makeMocks();
		EXPECT_CALL(*wrapperMockPos, equals_proxy(wrapperMockBase)).WillOnce(Return(true));
		EXPECT_CALL(*wrapperMockBase, increment());
		EXPECT_CALL(*enumerator, next());
		EXPECT_CALL(*enumerator, valid()).WillOnce(Return(true));
		EXPECT_CALL(*enumerator, current()).WillOnce(ReturnRef(nextValue));
		EXPECT_CALL(*elp, end()).WillOnce(ReturnIterator([endValue]() {
			return new EntryIteratorEndWrapper(endValue);
		}));
		EXPECT_CALL(*wrapperMockBase, equals_proxy(IsEnd(endValue))).WillOnce(Return(false));
		EXPECT_CALL(*wrapperMockBase, dereference()).WillOnce(ReturnRef(otherValue));
		EXPECT_CALL(*elp, find(Eq(ByRef(nextValue)))).WillOnce(Return(foundEntry));
		EXPECT_CALL(*wrapperMockBase, clone()).WillOnce(Return(wrapperMockClone));
		int iterations = ::Test::rand(UINT32_MAX) % 6;
		for (int i = 0; i <= iterations; i++)
		{
			EXPECT_CALL(*wrapperMockClone, dereference()).WillOnce(ReturnRef(otherValue));
			EXPECT_CALL(*wrapperMockClone, increment());
		}
		EXPECT_CALL(*wrapperMockClone, dereference()).WillOnce(ReturnRef(nextValue));
		EXPECT_CALL(*wrapperMockClone, clone()).WillOnce(Return(wrapperMockClone2));
		EXPECT_CALL(*elp, erase_proxy(HaveWrapper(wrapperMockClone2))).WillOnce(ReturnIterator([] {
			return nullptr;
		}));
		EXPECT_CALL(*wrapperMockBase, clone()).WillOnce(Return(wrapperMockClone3));
		EXPECT_CALL(*elp,
			insert_proxy(
				HaveWrapper(wrapperMockClone3), Eq(ByRef(nextValue)), Eq(ByRef(foundEntry))))
			.WillOnce(ReturnIterator([wrapperMockBase2]() { return wrapperMockBase2; }));
		EXPECT_CALL(*wrapperMockPos, increment());
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		EntryIterator combined(
			state.atCombined(EntryIterator(wrapperMockBase), EntryIterator(wrapperMockPos)));
		++combined;
	}

	TEST_F(EntryGeneratorTest, itIncCombinedChangeKeyMissing)
	{
		::testing::InSequence seq;
		const std::int64_t endValue = ::Test::rand(UINT32_MAX);
		fusekit::entry* const createdEntry = reinterpret_cast<decltype(createdEntry)>(::Test::rand(UINT32_MAX));
		const std::string nextValue = std::string("next") + std::to_string(::Test::rand(UINT32_MAX));
		const std::string otherValue = std::string("other") + std::to_string(::Test::rand(UINT32_MAX));
		EntryIteratorWrapperMock* wrapperMockBase = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockBase2 = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockPos = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockClone = new EntryIteratorWrapperMock;
		makeMocks();
		EXPECT_CALL(*wrapperMockPos, equals_proxy(wrapperMockBase)).WillOnce(Return(true));
		EXPECT_CALL(*wrapperMockBase, increment());
		EXPECT_CALL(*enumerator, next());
		EXPECT_CALL(*enumerator, valid()).WillOnce(Return(true));
		EXPECT_CALL(*enumerator, current()).WillOnce(ReturnRef(nextValue));
		EXPECT_CALL(*elp, end()).WillOnce(ReturnIterator([endValue]() {
			return new EntryIteratorEndWrapper(endValue);
		}));
		EXPECT_CALL(*wrapperMockBase, equals_proxy(IsEnd(endValue))).WillOnce(Return(false));
		EXPECT_CALL(*wrapperMockBase, dereference()).WillOnce(ReturnRef(otherValue));
		EXPECT_CALL(*elp, find(Eq(ByRef(nextValue)))).WillOnce(Return(nullptr));
		EXPECT_CALL(*factory, create(nextValue)).WillOnce(Return(createdEntry));
		EXPECT_CALL(*wrapperMockBase, clone()).WillOnce(Return(wrapperMockClone));
		EXPECT_CALL(*elp,
			insert_proxy(
				HaveWrapper(wrapperMockClone), Eq(ByRef(nextValue)), Eq(ByRef(createdEntry))))
			.WillOnce(ReturnIterator([wrapperMockBase2]() { return wrapperMockBase2; }));
		EXPECT_CALL(*wrapperMockPos, increment());
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		EntryIterator combined(
			state.atCombined(EntryIterator(wrapperMockBase), EntryIterator(wrapperMockPos)));
		++combined;
	}

	TEST_F(EntryGeneratorTest, itIncCombinedChangeNoMore)
	{
		::testing::InSequence seq;
		const std::int64_t endValue = ::Test::rand(UINT32_MAX);
		const std::string nextValue = std::string("next") + std::to_string(::Test::rand(UINT32_MAX));
		EntryIteratorWrapperMock* wrapperMockBase = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockPos = new EntryIteratorWrapperMock;
		std::vector<EntryIteratorWrapperMock*> wrappers;
		std::vector<EntryIteratorWrapperMock*> wrappersClone;
		std::vector<std::string> keys;
		std::vector<fusekit::entry*> entries;
		EntryIteratorWrapperMock* wrapperIt;
		makeMocks();
		EXPECT_CALL(*wrapperMockPos, equals_proxy(wrapperMockBase)).WillOnce(Return(true));
		EXPECT_CALL(*wrapperMockBase, increment());
		EXPECT_CALL(*enumerator, next());
		EXPECT_CALL(*enumerator, valid()).WillOnce(Return(false));
		int iterations = ::Test::rand(UINT32_MAX) % 6;
		wrapperIt = wrapperMockBase;
		wrappers.resize(iterations);
		wrappersClone.resize(iterations);
		keys.resize(iterations);
		entries.resize(iterations);
		for (int i = 0; i <= iterations; i++)
		{
			EXPECT_CALL(*elp, end()).WillOnce(ReturnIterator([endValue]() {
				return new EntryIteratorEndWrapper(endValue);
			}));
			EXPECT_CALL(*wrapperIt, equals_proxy(IsEnd(endValue)))
				.WillOnce(Return(i == iterations));
			if (i < iterations)
			{
				wrappers[i] = new EntryIteratorWrapperMock;
				wrappersClone[i] = new EntryIteratorWrapperMock;
				keys[i] = std::string("key") + std::to_string(::Test::rand(UINT32_MAX));
				entries[i] = reinterpret_cast<fusekit::entry*>(::Test::rand(UINT32_MAX));
				EXPECT_CALL(*wrapperIt, dereference()).WillOnce(ReturnRef(keys[i]));
				EXPECT_CALL(*elp, find(keys[i])).WillOnce(Return(entries[i]));
				EXPECT_CALL(*factory, destroy(entries[i]));
				EXPECT_CALL(*wrapperIt, clone()).WillOnce(Return(wrappersClone[i]));
				EXPECT_CALL(*elp, erase_proxy(HaveWrapper(wrappersClone[i])))
					.WillOnce(ReturnIterator([&wrappers, i]() { return wrappers[i]; }));
				wrapperIt = wrappers[i];
			}
		}
		EXPECT_CALL(*wrapperMockPos, increment());
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		EntryIterator combined(
			state.atCombined(EntryIterator(wrapperMockBase), EntryIterator(wrapperMockPos)));
		++combined;
	}

	TEST_F(EntryGeneratorTest, itIncCombinedChangeAtEnd)
	{
		::testing::InSequence seq;
		const std::int64_t endValue = ::Test::rand(UINT32_MAX);
		fusekit::entry* const createdEntry = reinterpret_cast<decltype(createdEntry)>(::Test::rand(UINT32_MAX));
		const std::string nextValue = std::string("next") + std::to_string(::Test::rand(UINT32_MAX));
		EntryIteratorWrapperMock* wrapperMockBase = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockBase2 = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockPos = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockClone = new EntryIteratorWrapperMock;
		makeMocks();
		EXPECT_CALL(*wrapperMockPos, equals_proxy(wrapperMockBase)).WillOnce(Return(true));
		EXPECT_CALL(*wrapperMockBase, increment());
		EXPECT_CALL(*enumerator, next());
		EXPECT_CALL(*enumerator, valid()).WillOnce(Return(true));
		EXPECT_CALL(*enumerator, current()).WillOnce(ReturnRef(nextValue));
		EXPECT_CALL(*elp, end()).WillOnce(ReturnIterator([endValue]() {
			return new EntryIteratorEndWrapper(endValue);
		}));
		EXPECT_CALL(*wrapperMockBase, equals_proxy(IsEnd(endValue))).WillOnce(Return(true));
		EXPECT_CALL(*factory, create(nextValue)).WillOnce(Return(createdEntry));
		EXPECT_CALL(*wrapperMockBase, clone()).WillOnce(Return(wrapperMockClone));
		EXPECT_CALL(*elp,
			insert_proxy(
				HaveWrapper(wrapperMockClone), Eq(ByRef(nextValue)), Eq(ByRef(createdEntry))))
			.WillOnce(ReturnIterator([wrapperMockBase2]() { return wrapperMockBase2; }));
		EXPECT_CALL(*wrapperMockPos, increment());
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		EntryIterator combined(
			state.atCombined(EntryIterator(wrapperMockBase), EntryIterator(wrapperMockPos)));
		++combined;
	}

	TEST_F(EntryGeneratorTest, itDirectCopy)
	{
		EntryIteratorWrapperMock* wrapperMockPos = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockClone = new EntryIteratorWrapperMock;
		makeMocks();
		EXPECT_CALL(*wrapperMockPos, clone()).WillOnce(Return(wrapperMockClone));
		EXPECT_CALL(*wrapperMockPos, equals_proxy(wrapperMockClone)).WillOnce(Return(true));
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		EntryIterator direct(state.atDirect(EntryIterator(wrapperMockPos)));
		EXPECT_EQ(state.allWrappers().size(), 1);
		auto clone = direct;
		EXPECT_EQ(direct, clone);
		EXPECT_EQ(state.allWrappers().size(), 2);
	}

	TEST_F(EntryGeneratorTest, itCombinedCopy)
	{
		EntryIteratorWrapperMock* wrapperMockBase = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockPos = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockClone = new EntryIteratorWrapperMock;
		makeMocks();
		EXPECT_CALL(*wrapperMockPos, clone()).WillOnce(Return(wrapperMockClone));
		EXPECT_CALL(*wrapperMockPos, equals_proxy(wrapperMockClone)).WillOnce(Return(true));
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		EntryIterator combined(
			state.atCombined(EntryIterator(wrapperMockBase), EntryIterator(wrapperMockPos)));
		EXPECT_EQ(state.allWrappers().size(), 1);
		auto clone = combined;
		EXPECT_EQ(combined, clone);
		EXPECT_EQ(state.allWrappers().size(), 2);
	}

	TEST_F(EntryGeneratorTest, itDirectDereference)
	{
		const std::string usedValue = std::string("used") + std::to_string(::Test::rand(UINT32_MAX));
		EntryIteratorWrapperMock* wrapperMockPos = new EntryIteratorWrapperMock;
		makeMocks();
		EXPECT_CALL(*wrapperMockPos, dereference()).WillOnce(ReturnRef(usedValue));
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		EntryIterator direct(state.atDirect(EntryIterator(wrapperMockPos)));
		EXPECT_EQ(*direct, usedValue);
	}

	TEST_F(EntryGeneratorTest, itCombinedDereference)
	{
		const std::string usedValue = std::string("used") + std::to_string(::Test::rand(UINT32_MAX));
		EntryIteratorWrapperMock* wrapperMockBase = new EntryIteratorWrapperMock;
		EntryIteratorWrapperMock* wrapperMockPos = new EntryIteratorWrapperMock;
		makeMocks();
		EXPECT_CALL(*wrapperMockPos, dereference()).WillOnce(ReturnRef(usedValue));
		ENTRY_GENERATOR(eg, elp, changed, enumerator, factory);
		Fixtures::HolderStateInitializer state(eg);
		EntryIterator combined(
			state.atCombined(EntryIterator(wrapperMockBase), EntryIterator(wrapperMockPos)));
		EXPECT_EQ(*combined, usedValue);
	}
} // namespace Test::ZipDirFs::Containers
