// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/c_ptr.hpp>
#include <primal/c_ptr.hpp>

#include <utility>

#include <doctest.h>

namespace
{
	struct Value
	{
		unsigned _counter = 0;
	};

	void increment(Value* pointer)
	{
		REQUIRE(pointer);
		++pointer->_counter;
	}

	using RawPtr = Value*;
	using CPtr = primal::CPtr<Value, increment>;
}

TEST_CASE("CPtr::CPtr()")
{
	CPtr ptr;
	CHECK(!ptr);
	CHECK(RawPtr{ ptr } == RawPtr{});
	CHECK(ptr.get() == RawPtr{});
	REQUIRE(ptr.out());
	CHECK(!*ptr.out());
}

TEST_CASE("CPtr::CPtr(T*)")
{
	Value value;
	{
		CPtr ptr{ &value };
		CHECK(ptr);
		CHECK(RawPtr{ ptr } == &value);
		CHECK(ptr.get() == &value);
		REQUIRE(ptr.out());
		CHECK(*ptr.out() == &value);
		CHECK(&ptr->_counter == &value._counter);
		CHECK(value._counter == 0);
	}
	CHECK(value._counter == 1);
}

TEST_CASE("CPtr::CPtr(CPtr&&)")
{
	Value value;
	{
		CPtr first{ &value };
		CHECK(first);
		CPtr second{ std::move(first) };
		CHECK(!first);
		CHECK(second);
		CHECK(value._counter == 0);
	}
	CHECK(value._counter == 1);
}

TEST_CASE("CPtr::operator=(CPtr&&)")
{
	Value value;
	{
		CPtr first{ &value };
		CHECK(first);
		CPtr second;
		CHECK(!second);
		second = std::move(first);
		CHECK(!first);
		CHECK(second);
		CHECK(value._counter == 0);
	}
	CHECK(value._counter == 1);
}
