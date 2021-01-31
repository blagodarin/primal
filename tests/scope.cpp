// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/scope.hpp>

#include <doctest.h>

TEST_CASE("FINALLY")
{
	unsigned value = 0;
	SUBCASE("leave")
	{
		{
			PRIMAL_FINALLY([&value] { value += 1; });
			PRIMAL_FINALLY([&value] { value += 2; });
			CHECK(value == 0);
		}
		CHECK(value == 3);
	}
	SUBCASE("throw")
	{
		bool caught = false;
		try
		{
			PRIMAL_FINALLY([&value] { value += 1; });
			PRIMAL_FINALLY([&value] { value += 2; });
			CHECK(value == 0);
			throw 0;
		}
		catch (int)
		{
			caught = true;
			CHECK(value == 3);
		}
		CHECK(caught);
	}
}
