// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/buffer.hpp>

#include <numeric>
#include <vector>

#include <doctest.h>

namespace
{
	using Buffer = primal::Buffer<int, primal::CleanAllocator<primal::Allocator>>;

	void check(Buffer& buffer, const std::vector<int>& expected, bool allocated = true)
	{
		CHECK(buffer.size() == expected.size());
		auto data = buffer.data();
		CHECK(std::as_const(buffer).data() == data);
		if (!expected.empty())
		{
			REQUIRE(data);
			for (size_t i = 0; i < buffer.size(); ++i)
			{
				INFO('[', i, ']');
				CHECK(data[i] == expected[i]);
			}
		}
		else if (!allocated)
			CHECK_FALSE(data);
	}
}

TEST_CASE("Buffer::Buffer(0)")
{
	Buffer buffer{ 0 };
	::check(buffer, {});
}

TEST_CASE("Buffer::Buffer(Buffer&&)")
{
	Buffer buffer{ 3 };
	::check(buffer, { 0, 0, 0 });
	std::iota(buffer.data(), buffer.data() + buffer.size(), 1);
	Buffer otherBuffer{ std::move(buffer) };
	::check(buffer, {}, false);
	::check(otherBuffer, { 1, 2, 3 });
}

TEST_CASE("Buffer::operator=(Buffer&&)")
{
	Buffer buffer{ 3 };
	::check(buffer, { 0, 0, 0 });
	std::iota(buffer.data(), buffer.data() + buffer.size(), 1);
	Buffer otherBuffer;
	::check(otherBuffer, {}, false);
	otherBuffer = std::move(buffer);
	::check(buffer, {}, true);
	::check(otherBuffer, { 1, 2, 3 });
}

TEST_CASE("Buffer::reallocate()")
{
	Buffer buffer;
	::check(buffer, {}, false);
	buffer.reallocate(3);
	::check(buffer, { 0, 0, 0 });
	std::iota(buffer.data(), buffer.data() + buffer.size(), 1);
	buffer.reallocate(4);
	::check(buffer, { 1, 2, 3, 0 });
	buffer.reallocate(5, false);
	::check(buffer, { 0, 0, 0, 0, 0 });
}

TEST_CASE("swap(Buffer&, Buffer&)")
{
	Buffer first{ 3 };
	::check(first, { 0, 0, 0 });
	std::iota(first.data(), first.data() + first.size(), 1);
	Buffer second{ 4 };
	::check(second, { 0, 0, 0, 0 });
	std::iota(second.data(), second.data() + second.size(), 4);
	swap(first, second);
	::check(first, { 4, 5, 6, 7 });
	::check(second, { 1, 2, 3 });
}
