// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/utf8.hpp>

#include <string>

#include <doctest.h>

TEST_CASE("isUtf8Continuation")
{
	unsigned byte = 0;
	for (; byte < 0x80; ++byte)
		CHECK(!primal::isUtf8Continuation(static_cast<char>(byte)));
	for (; byte < 0xc0; ++byte)
		CHECK(primal::isUtf8Continuation(static_cast<char>(byte)));
	for (; byte <= 0xff; ++byte)
		CHECK(!primal::isUtf8Continuation(static_cast<char>(byte)));
}

TEST_CASE("readUtf8")
{
	const auto readUtf8 = [](const std::string& string) -> std::pair<char32_t, size_t> {
		const std::string garbage{ '\xff' };
		auto offset = garbage.size();
		const auto codepoint = primal::readUtf8(garbage + string, offset);
		return { codepoint, offset - garbage.size() };
	};
	SUBCASE("01")
	{
		const auto [codepoint, advance] = readUtf8("\x01");
		CHECK(codepoint == 0x01);
		CHECK(advance == 1);
	}
	SUBCASE("7f")
	{
		const auto [codepoint, advance] = readUtf8("\x7f");
		CHECK(codepoint == 0x7f);
		CHECK(advance == 1);
	}
	SUBCASE("80")
	{
		const auto [codepoint, advance] = readUtf8("\xc2\x80");
		CHECK(codepoint == 0x80);
		CHECK(advance == 2);
	}
	SUBCASE("7ff")
	{
		const auto [codepoint, advance] = readUtf8("\xdf\xbf");
		CHECK(codepoint == 0x7ff);
		CHECK(advance == 2);
	}
	SUBCASE("800")
	{
		const auto [codepoint, advance] = readUtf8("\xe0\xa0\x80");
		CHECK(codepoint == 0x800);
		CHECK(advance == 3);
	}
	SUBCASE("ffff")
	{
		const auto [codepoint, advance] = readUtf8("\xef\xbf\xbf");
		CHECK(codepoint == 0xffff);
		CHECK(advance == 3);
	}
	SUBCASE("10000")
	{
		const auto [codepoint, advance] = readUtf8("\xf0\x90\x80\x80");
		CHECK(codepoint == 0x10000);
		CHECK(advance == 4);
	}
	SUBCASE("10ffff")
	{
		const auto [codepoint, advance] = readUtf8("\xf4\x8f\xbf\xbf");
		CHECK(codepoint == 0x10ffff);
		CHECK(advance == 4);
	}
}

TEST_CASE("writeUtf8")
{
	const auto writeUtf8 = [](char32_t codepoint) {
		std::array<char, 4> buffer;
		const auto size = primal::writeUtf8(buffer, codepoint);
		return size <= buffer.size() ? std::string{ buffer.data(), size } : std::string{};
	};
	CHECK(writeUtf8(0x0) == std::string{ '\0' });
	CHECK(writeUtf8(0x1) == "\x01");
	CHECK(writeUtf8(0x7f) == "\x7f");
	CHECK(writeUtf8(0x80) == "\xc2\x80");
	CHECK(writeUtf8(0x7ff) == "\xdf\xbf");
	CHECK(writeUtf8(0x800) == "\xe0\xa0\x80");
	CHECK(writeUtf8(0xffff) == "\xef\xbf\xbf");
	CHECK(writeUtf8(0x10000) == "\xf0\x90\x80\x80");
	CHECK(writeUtf8(0x10ffff) == "\xf4\x8f\xbf\xbf");
	CHECK(writeUtf8(0x110000) == "");
	CHECK(writeUtf8(0xffffffff) == "");
}
