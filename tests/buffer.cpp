// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/buffer.hpp>

#include <numeric>

#include <doctest.h>

TEST_CASE("Buffer::Buffer()")
{
	primal::Buffer<int> buffer;
	CHECK(buffer.size() == 0);
	CHECK(!buffer.data());
}

TEST_CASE("Buffer::Buffer(size_t)")
{
	primal::Buffer<int> buffer{ 3 };
	CHECK(buffer.size() == 3);
	REQUIRE(buffer.data());
	std::iota(buffer.data(), buffer.data() + buffer.size(), 1);
	CHECK(buffer[0] == 1);
	CHECK(buffer[1] == 2);
	CHECK(buffer[2] == 3);
}
