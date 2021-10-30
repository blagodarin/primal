// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/endian.hpp>

#include <array>
#include <cstring>

#include <doctest/doctest.h>

TEST_CASE("bigEndian(T)")
{
	if constexpr (std::endian::native == std::endian::little)
	{
		CHECK(primal::bigEndian(uint16_t{ 0x8081 }) == 0x8180);
		CHECK(primal::bigEndian(uint32_t{ 0x80818283 }) == 0x83828180);
	}
	if constexpr (std::endian::native == std::endian::big)
	{
		CHECK(primal::bigEndian(uint16_t{ 0x8081 }) == 0x8081);
		CHECK(primal::bigEndian(uint32_t{ 0x80818283 }) == 0x80818283);
	}
}

TEST_CASE("littleEndian(T)")
{
	if constexpr (std::endian::native == std::endian::little)
	{
		CHECK(primal::littleEndian(uint16_t{ 0x8081 }) == 0x8081);
		CHECK(primal::littleEndian(uint32_t{ 0x80818283 }) == 0x80818283);
	}
	if constexpr (std::endian::native == std::endian::big)
	{
		CHECK(primal::littleEndian(uint16_t{ 0x8081 }) == 0x8180);
		CHECK(primal::littleEndian(uint32_t{ 0x80818283 }) == 0x83828180);
	}
}

TEST_CASE("makeCC(char, char)")
{
	constexpr auto value = primal::makeCC('\x01', '\xff');
	CHECK(std::memcmp(&value, "\x01\xff", sizeof value) == 0);
}

TEST_CASE("makeCC(char, char, char, char)")
{
	constexpr auto value = primal::makeCC('\x01', '\x80', '\x7f', '\xff');
	CHECK(std::memcmp(&value, "\x01\x80\x7f\xff", sizeof value) == 0);
}

TEST_CASE("makeCC(char, char, char, char, char, char, char, char)")
{
	constexpr auto value = primal::makeCC('\x01', '\x02', '\x7e', '\x7f', '\x80', '\x81', '\xfe', '\xff');
	CHECK(std::memcmp(&value, "\x01\x02\x7e\x7f\x80\x81\xfe\xff", sizeof value) == 0);
}

TEST_CASE("swapBytes(uint16_t)")
{
	CHECK(primal::swapBytes(uint16_t{ 0x8081 }) == 0x8180);
}

TEST_CASE("swapBytes(uint32_t)")
{
	CHECK(primal::swapBytes(uint32_t{ 0x80818283 }) == 0x83828180);
}
