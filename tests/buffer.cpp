// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/buffer.hpp>

#include <numeric>
#include <vector>

#include <doctest.h>

namespace
{
	struct CleanAllocator
	{
		static void* allocate(size_t size)
		{
			const auto result = primal::Allocator::allocate(size);
			std::memset(result, 0, size);
			return result;
		}

		static void deallocate(void* memory) noexcept
		{
			primal::Allocator::deallocate(memory);
		}
	};

	using Buffer = primal::Buffer<int, CleanAllocator>;

	void check(const Buffer& buffer, const std::vector<int>& expectedData)
	{
		REQUIRE(buffer.size() == expectedData.size());
		for (size_t i = 0; i < buffer.size(); ++i)
		{
			INFO('[', i, ']');
			CHECK(buffer.data()[i] == expectedData[i]);
		}
	}
}

TEST_CASE("Buffer::Buffer(size_t)")
{
	Buffer buffer{ 3 };
	CHECK(buffer.data());
	::check(buffer, { 0, 0, 0 });
}

TEST_CASE("Buffer::resize()")
{
	Buffer buffer;
	CHECK(!buffer.data());
	::check(buffer, {});
	buffer.resize(3);
	REQUIRE(buffer.data());
	std::iota(buffer.data(), buffer.data() + buffer.size(), 1);
	buffer.resize(4);
	::check(buffer, { 1, 2, 3, 0 });
	buffer.resize(5, false);
	::check(buffer, { 0, 0, 0, 0, 0 });
}
