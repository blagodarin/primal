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

	void checkData(const Buffer& buffer, const std::vector<int>& expectedData)
	{
		CHECK(buffer.size() == expectedData.size());
		if (!expectedData.empty())
		{
			REQUIRE(buffer.data());
			for (size_t i = 0; i < buffer.size(); ++i)
			{
				INFO('[', i, ']');
				CHECK(buffer.data()[i] == expectedData[i]);
			}
		}
	}

	void checkNull(const Buffer& buffer)
	{
		CHECK(!buffer.data());
		CHECK(buffer.size() == 0);
	}
}

TEST_CASE("Buffer::Buffer(0)")
{
	Buffer buffer{ 0 };
	::checkData(buffer, {});
}

TEST_CASE("Buffer::Buffer(Buffer&&)")
{
	Buffer buffer{ 3 };
	::checkData(buffer, { 0, 0, 0 });
	std::iota(buffer.data(), buffer.data() + buffer.size(), 1);
	Buffer otherBuffer{ std::move(buffer) };
	::checkNull(buffer);
	::checkData(otherBuffer, { 1, 2, 3 });
}

TEST_CASE("Buffer::operator=(Buffer&&)")
{
	Buffer buffer{ 3 };
	::checkData(buffer, { 0, 0, 0 });
	std::iota(buffer.data(), buffer.data() + buffer.size(), 1);
	Buffer otherBuffer;
	::checkNull(otherBuffer);
	otherBuffer = std::move(buffer);
	::checkNull(buffer);
	::checkData(otherBuffer, { 1, 2, 3 });
}

TEST_CASE("Buffer::reallocate()")
{
	Buffer buffer;
	::checkNull(buffer);
	buffer.reallocate(3);
	::checkData(buffer, { 0, 0, 0 });
	std::iota(buffer.data(), buffer.data() + buffer.size(), 1);
	buffer.reallocate(4);
	::checkData(buffer, { 1, 2, 3, 0 });
	buffer.reallocate(5, false);
	::checkData(buffer, { 0, 0, 0, 0, 0 });
}

TEST_CASE("swap(Buffer&, Buffer&)")
{
	Buffer first{ 3 };
	::checkData(first, { 0, 0, 0 });
	std::iota(first.data(), first.data() + first.size(), 1);
	Buffer second{ 4 };
	::checkData(second, { 0, 0, 0, 0 });
	std::iota(second.data(), second.data() + second.size(), 4);
	swap(first, second);
	::checkData(first, { 4, 5, 6, 7 });
	::checkData(second, { 1, 2, 3 });
}
