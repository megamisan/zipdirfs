/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "EntryIterator.h"
#include "ZipDirFs/Containers/EntryIterator.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <type_traits>

namespace ZipDirFs::Containers
{
	template <typename _Tp>
	bool operator!=(const _Tp& a, const _Tp& b)
	{
		return !(a == b);
	};
} // namespace ZipDirFs::Containers

namespace Test::ZipDirFs::Containers
{
	using ::testing::InSequence;
	using ::testing::Return;
	using ::testing::ReturnRef;
	using Manager = Fixtures::EntryIteratorMockManagerHelper;
	using Create = Fixtures::EntryIteratorMockCreateHelper;
	using Destruct = Fixtures::EntryIteratorMockDestructHelper;
	using WrapperMock = Fixtures::EntryIteratorWrapperMock;

	EntryIterator::pointer_wrapper& EntryIteratorAccess::getWrapper(EntryIterator& ei)
	{
		return reinterpret_cast<EntryIteratorAccess*>(&ei)->wrapper;
	}

	WrapperMock* EntryIteratorTest::getWrapper(EntryIterator& ei)
	{
		return reinterpret_cast<WrapperMock*>(EntryIteratorAccess::getWrapper(ei).get());
	}
	void EntryIteratorTest::setWrapper(EntryIterator& ei, EntryIterator::Wrapper* w)
	{
		EntryIteratorAccess::getWrapper(ei) = EntryIterator::pointer_wrapper(w);
	}

	TEST_F(EntryIteratorTest, TypeCheck)
	{
		EXPECT_TRUE(std::is_default_constructible<EntryIterator>::value);
		EXPECT_TRUE(std::is_move_constructible<EntryIterator>::value);
		EXPECT_TRUE(std::is_copy_constructible<EntryIterator>::value);
		EXPECT_TRUE(std::is_move_assignable<EntryIterator>::value);
		EXPECT_TRUE(std::is_copy_assignable<EntryIterator>::value);
		EXPECT_TRUE(std::is_destructible<EntryIterator>::value);
		EXPECT_TRUE(std::__is_swappable<EntryIterator>::value);
	}

	ACTION_P(ReturnNew, output) { return *output = new WrapperMock(); }

	TEST_F(EntryIteratorTest, InitEmpty)
	{
		Create c;
		EntryIterator ei;
		EXPECT_EQ(nullptr, getWrapper(ei));
	}

	TEST_F(EntryIteratorTest, InitNew)
	{
		Manager m;
		auto* peiwt = new WrapperMock();
		Create c;
		EntryIterator ei(peiwt);
		EXPECT_EQ(peiwt, getWrapper(ei));
	}

	TEST_F(EntryIteratorTest, InitMove)
	{
		Manager m;
		auto* peiwt = new WrapperMock();
		Create c;
		EntryIterator ei(std::move(EntryIterator(peiwt)));
		EXPECT_EQ(peiwt, getWrapper(ei));
	}

	TEST_F(EntryIteratorTest, InitCopy)
	{
		Manager m;
		auto* peiwt = new WrapperMock();
		WrapperMock* peiwt2;
		EXPECT_CALL(*peiwt, clone()).WillOnce(ReturnNew(&peiwt2));
		EntryIterator ei;
		setWrapper(ei, peiwt);
		EntryIterator ei2(ei);
		EXPECT_EQ(peiwt, getWrapper(ei));
		EXPECT_EQ(peiwt2, getWrapper(ei2));
	}

	TEST_F(EntryIteratorTest, AssignMoveFromEmpty)
	{
		Manager m;
		auto* peiwt = new WrapperMock();
		Create c;
		EntryIterator ei;
		ei = std::move(EntryIterator(peiwt));
		EXPECT_EQ(peiwt, getWrapper(ei));
	}

	TEST_F(EntryIteratorTest, AssignMoveFromSet)
	{
		Manager m;
		auto* peiwt = new WrapperMock();
		Destruct d;
		EntryIterator ei;
		setWrapper(ei, peiwt);
		EXPECT_CALL(d, destroy(peiwt));
		ei = std::move(EntryIterator());
		EXPECT_EQ(nullptr, getWrapper(ei));
	}

	TEST_F(EntryIteratorTest, AssignCopyFromEmpty)
	{
		Manager m;
		auto* peiwt = new WrapperMock();
		WrapperMock* peiwt2;
		EXPECT_CALL(*peiwt, clone()).WillOnce(ReturnNew(&peiwt2));
		EntryIterator ei;
		EntryIterator ei2;
		setWrapper(ei, peiwt);
		ei2 = ei;
		EXPECT_EQ(peiwt, getWrapper(ei));
		EXPECT_EQ(peiwt2, getWrapper(ei2));
	}

	TEST_F(EntryIteratorTest, AssignCopyFromSet)
	{
		Manager m;
		InSequence seq;
		auto* peiwt = new WrapperMock();
		auto* peiwt2 = new WrapperMock();
		WrapperMock* peiwt3;
		EXPECT_CALL(*peiwt, clone()).WillOnce(ReturnNew(&peiwt3));
		EntryIterator ei;
		EntryIterator ei2;
		Destruct d;
		EXPECT_CALL(d, destroy(peiwt2));
		setWrapper(ei, peiwt);
		setWrapper(ei2, peiwt2);
		ei2 = ei;
		EXPECT_EQ(peiwt, getWrapper(ei));
		EXPECT_EQ(peiwt3, getWrapper(ei2));
	}

	TEST_F(EntryIteratorTest, AssignCopyToEmpty)
	{
		Manager m;
		Destruct d;
		auto* peiwt2 = new WrapperMock();
		EntryIterator ei;
		EntryIterator ei2;
		setWrapper(ei2, peiwt2);
		EXPECT_CALL(d, destroy(peiwt2));
		ei2 = ei;
		EXPECT_EQ(nullptr, getWrapper(ei2));
	}

	TEST_F(EntryIteratorTest, Destroy)
	{
		Manager m;
		Destruct d;
		auto peiwt = new WrapperMock();
		EntryIterator ei;
		setWrapper(ei, peiwt);
		EXPECT_CALL(d, destroy(peiwt));
	}

	TEST_F(EntryIteratorTest, Dereference)
	{
		Manager m;
		auto peiwt = new WrapperMock();
		EntryIterator ei;
		setWrapper(ei, peiwt);
		std::string value;
		EXPECT_CALL(*peiwt, dereference()).WillOnce(ReturnRef(value));
		EXPECT_EQ(&value, &(*ei));
	}

	TEST_F(EntryIteratorTest, Increment)
	{
		Manager m;
		auto peiwt = new WrapperMock();
		EntryIterator ei;
		setWrapper(ei, peiwt);
		EXPECT_CALL(*peiwt, increment());
		++ei;
	}

	TEST_F(EntryIteratorTest, EqualsEmpty)
	{
		EntryIterator ei1;
		EntryIterator ei2;
		EXPECT_EQ(ei1, ei2);
	}

	TEST_F(EntryIteratorTest, EqualsLeftEmpty)
	{
		Manager m;
		EntryIterator ei1;
		EntryIterator ei2;
		setWrapper(ei2, new WrapperMock());
		EXPECT_NE(ei1, ei2);
	}

	TEST_F(EntryIteratorTest, EqualsRightEmpty)
	{
		Manager m;
		EntryIterator ei1;
		EntryIterator ei2;
		setWrapper(ei1, new WrapperMock());
		EXPECT_NE(ei1, ei2);
	}

	TEST_F(EntryIteratorTest, EqualsBoth)
	{
		Manager m;
		auto peiwt1 = new WrapperMock();
		auto peiwt2 = new WrapperMock();
		EntryIterator ei1;
		EntryIterator ei2;
		setWrapper(ei1, peiwt1);
		setWrapper(ei2, peiwt2);
		EXPECT_CALL(*peiwt1, equals_proxy(peiwt2));
		ei1 == ei2;
	}

	TEST_F(EntryIteratorTest, OpEquals)
	{
		WrapperMock w1;
		WrapperMock w2;
		EXPECT_CALL(w1, equals_proxy(&w2));
		w1 == w2;
	}

	TEST_F(EntryIteratorTest, SwapLocal)
	{
		Manager m;
		EntryIterator::Wrapper* peiwt1 = new WrapperMock;
		EntryIterator::Wrapper* peiwt2 = new WrapperMock;
		EntryIterator ei1(peiwt1);
		EntryIterator ei2(peiwt2);
		ei1.swap(ei2);
		EXPECT_EQ(peiwt2, ei1.getWrapper());
		EXPECT_EQ(peiwt1, ei2.getWrapper());
	}

	TEST_F(EntryIteratorTest, SwapGlobal)
	{
		Manager m;
		EntryIterator::Wrapper* peiwt1 = new WrapperMock;
		EntryIterator::Wrapper* peiwt2 = new WrapperMock;
		EntryIterator ei1(peiwt1);
		EntryIterator ei2(peiwt2);
		std::swap(ei1, ei2);
		EXPECT_EQ(peiwt2, ei1.getWrapper());
		EXPECT_EQ(peiwt1, ei2.getWrapper());
	}
} // namespace Test::ZipDirFs::Containers
