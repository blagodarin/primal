// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/buffer.hpp>
#include <primal/buffer.hpp>

#include <doctest.h>

TEST_CASE("Buffer::Buffer()")
{
	primal::Buffer<int> buffer;
	CHECK(!buffer.data());
	CHECK(buffer.size() == 0);
}

TEST_CASE("Buffer::Buffer(size_t)")
{
	primal::Buffer<int> buffer{ 2 };
	REQUIRE(buffer.data());
	CHECK(buffer.size() == 2);
	buffer[0] = 1;
	buffer[1] = 2;
	CHECK(buffer.data()[0] == 1);
	CHECK(buffer.data()[1] == 2);
	buffer[0] = 3;
	buffer[1] = 4;
	CHECK(buffer.data()[0] == 3);
	CHECK(buffer.data()[1] == 4);
}
