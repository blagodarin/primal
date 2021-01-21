// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/buffer.hpp>
#include <primal/buffer.hpp>

#include <limits>
#include <numeric>

#include <doctest.h>

TEST_CASE("Buffer::Buffer()")
{
	primal::Buffer<int> buffer;
	CHECK(!buffer.data());
	CHECK(buffer.size() == 0);
}

TEST_CASE("Buffer::Buffer(3)")
{
	primal::Buffer<int> buffer{ 3 };
	REQUIRE(buffer.data());
	CHECK(buffer.size() == 3);
	std::iota(buffer.data(), buffer.data() + buffer.size(), 1);
	CHECK(buffer[0] == 1);
	CHECK(buffer[1] == 2);
	CHECK(buffer[2] == 3);
}

TEST_CASE("Buffer::Buffer(SIZE_MAX)")
{
	CHECK_THROWS_AS(primal::Buffer<int> buffer{ std::numeric_limits<size_t>::max() }, std::bad_alloc);
}
