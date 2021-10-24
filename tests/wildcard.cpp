// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/wildcard.hpp>

#include <string>

#include <doctest.h>

TEST_CASE("matchWildcard")
{
	// To prevent compile time evaluation.
	const auto matchWildcard = [](const std::string& text, const std::string& wildcard) {
		return primal::matchWildcard(text, wildcard);
	};
	SUBCASE("a")
	{
		CHECK(matchWildcard("", ""));
		CHECK(matchWildcard("abc", "abc"));
		CHECK(!matchWildcard("", "abc"));
		CHECK(!matchWildcard("abc", ""));
		CHECK(!matchWildcard("abc", "abcd"));
		CHECK(!matchWildcard("abc", "abd"));
		CHECK(!matchWildcard("abcd", "abc"));
	}
	SUBCASE("?")
	{
		CHECK(matchWildcard("a", "?"));
		CHECK(matchWildcard("abc", "ab?"));
		CHECK(matchWildcard("abc", "?bc"));
		CHECK(matchWildcard("abc", "???"));
		CHECK(!matchWildcard("", "?"));
		CHECK(!matchWildcard("a", "??"));
		CHECK(!matchWildcard("abc", "?"));
		CHECK(!matchWildcard("abc", "??"));
		CHECK(!matchWildcard("abc", "abc?"));
		CHECK(!matchWildcard("abc", "?abc"));
		CHECK(!matchWildcard("abc", "????"));
	}
	SUBCASE("*")
	{
		CHECK(matchWildcard("", "*"));
		CHECK(matchWildcard("", "**"));
		CHECK(matchWildcard("abc", "*"));
		CHECK(matchWildcard("abc", "**"));
		CHECK(matchWildcard("abc", "a*"));
		CHECK(matchWildcard("abc", "*b*"));
		CHECK(matchWildcard("abc", "*c"));
		CHECK(matchWildcard("abc", "abc*"));
		CHECK(matchWildcard("abc", "*abc"));
		CHECK(!matchWildcard("abc", "bc*"));
		CHECK(!matchWildcard("abc", "*ab"));
	}
	SUBCASE("abc*def*fgh")
	{
		using namespace std::string_literals;
		CHECK(matchWildcard("abc"s + "def" + "fgh", "abc*def*fgh"));
		CHECK(matchWildcard("abc"s + "xyz" + "def" + "xyz" + "fgh", "abc*def*fgh"));
		CHECK(matchWildcard("abc"s + "de" + "def" + "fgh", "abc*def*fgh"));
		CHECK(matchWildcard("abc"s + "def" + "def" + "fgh", "abc*def*fgh"));
		CHECK(matchWildcard("abc"s + "def" + "fgh" + "fgh", "abc*def*fgh"));
		CHECK(!matchWildcard("abc"s + "de" + "fgh", "abc*def*fgh"));
		CHECK(!matchWildcard("abc"s + "def" + "fgh" + "xyz", "abc*def*fgh"));
	}
}
