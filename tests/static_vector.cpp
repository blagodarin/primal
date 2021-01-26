// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/static_vector.hpp>

#include <vector>

#include <doctest.h>

namespace
{
	constexpr size_t kCapacity = 3;

	using StaticVector = primal::StaticVector<int, kCapacity>;

	void check(const StaticVector& vector, const std::vector<int>& expected)
	{
		CHECK(vector.empty() == expected.empty());
		CHECK(vector.size() == expected.size());
		auto it = vector.begin();
		auto cit = vector.begin();
		for (size_t index = 0; index < expected.size(); ++index)
		{
			INFO('[', index, ']');
			REQUIRE(it < vector.end());
			REQUIRE(cit < vector.cend());
			CHECK(*it == expected[index]);
			CHECK(*cit == expected[index]);
			++it;
			++cit;
		}
		REQUIRE(it == vector.end());
		REQUIRE(cit == vector.cend());
	}
}

TEST_CASE("StaticVector")
{
	StaticVector vector;
	::check(vector, {});
	vector.emplace_back(1);
	::check(vector, { 1 });
	vector.emplace_back(2);
	vector.emplace_back(3);
	::check(vector, { 1, 2, 3 });
	vector.pop_back();
	::check(vector, { 1, 2 });
	vector.clear();
	::check(vector, {});
}
