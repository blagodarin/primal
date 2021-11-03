// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/dsp.hpp>

#include <array>
#include <numeric>

#include <doctest/doctest.h>

namespace
{
	template <typename T, size_t N>
	constexpr bool checkSize(const std::array<T, N>&) noexcept
	{
		return N * sizeof(T) > primal::kDspAlignment && N * sizeof(T) % primal::kDspAlignment != 0;
	}
}

TEST_CASE("addSaturate1D")
{
	alignas(primal::kDspAlignment) std::array<float, 15> first{
		-1.f, -.875f, -.75f, -.625f, -.5f, -.375f, -.25f, -.125f,
		0.f, .125f, .25f, .375f, .5f, .625f, .75f
	};
	static_assert(::checkSize(first));
	alignas(primal::kDspAlignment) const std::array<float, first.size()> second{
		-.875f, -.75f, -.625f, -.5f, -.375f, -.25f, -.125f, -0.f,
		.125f, .25f, .375f, .5f, .625f, .75f, .875f
	};
	const std::array<float, first.size()> expected{
		-1.f, -1.f, -1.f, -1.f, -.875f, -.625f, -.375f, -.125f,
		.125f, .375f, .625f, .875f, 1.f, 1.f, 1.f
	};
	primal::addSaturate1D(first.data(), second.data(), first.size());
	for (size_t i = 0; i < first.size(); ++i)
		CHECK(first[i] == expected[i]);
}

TEST_CASE("duplicate1D_16")
{
	constexpr size_t length = (2 * primal::kDspAlignment - 1) / sizeof(int16_t);
	alignas(primal::kDspAlignment) std::array<int16_t, length> input{};
	static_assert(::checkSize(input));
	std::iota(input.begin(), input.end(), int16_t{ 1 });
	alignas(primal::kDspAlignment) std::array<int16_t, length * 2> output{};
	primal::duplicate1D_16(output.data(), input.data(), length);
	for (size_t i = 0; i < output.size(); ++i)
		CHECK(output[i] == input[i / 2]);
}

TEST_CASE("duplicate1D_32")
{
	constexpr size_t length = (2 * primal::kDspAlignment - 1) / sizeof(float);
	alignas(primal::kDspAlignment) std::array<float, length> input{};
	static_assert(::checkSize(input));
	std::iota(input.begin(), input.end(), 1.f);
	alignas(primal::kDspAlignment) std::array<float, length * 2> output{};
	primal::duplicate1D_32(output.data(), input.data(), length);
	for (size_t i = 0; i < output.size(); ++i)
		CHECK(output[i] == input[i / 2]);
}

TEST_CASE("normalize1D")
{
	alignas(primal::kDspAlignment) std::array<int16_t, 15> input{
		-32768, -28672, -24576, -20480, -16384, -12288, -8192, -4096,
		0, 4096, 8192, 12288, 16384, 20480, 24576
	};
	static_assert(::checkSize(input));
	const std::array<float, input.size()> expected{
		-1.f, -.875f, -.75f, -.625f, -.5f, -.375f, -.25f, -.125f,
		0.f, .125f, .25f, .375f, .5f, .625f, .75f
	};
	static_assert(::checkSize(expected));
	alignas(primal::kDspAlignment) std::array<float, expected.size()> actual{};
	primal::normalize1D(actual.data(), input.data(), input.size());
	for (size_t i = 0; i < actual.size(); ++i)
		CHECK(actual[i] == expected[i]);
}

TEST_CASE("normalizeDuplicate1D")
{
	alignas(primal::kDspAlignment) std::array<int16_t, 15> input{
		-32768, -28672, -24576, -20480, -16384, -12288, -8192, -4096,
		0, 4096, 8192, 12288, 16384, 20480, 24576
	};
	static_assert(::checkSize(input));
	std::array<float, input.size() * 2> expected{
		-1.000f, -1.000f, -0.875f, -0.875f, -0.750f, -0.750f, -0.625f, -0.625f,
		-0.500f, -0.500f, -0.375f, -0.375f, -0.250f, -0.250f, -0.125f, -0.125f,
		0.000f, 0.000f, 0.125f, 0.125f, 0.250f, 0.250f, 0.375f,
		0.375f, 0.500f, 0.500f, 0.625f, 0.625f, 0.750f, 0.750f
	};
	static_assert(::checkSize(expected));
	alignas(primal::kDspAlignment) std::array<float, expected.size()> actual{};
	primal::normalizeDuplicate1D(actual.data(), input.data(), input.size());
	for (size_t i = 0; i < actual.size(); ++i)
		CHECK(actual[i] == expected[i]);
}
