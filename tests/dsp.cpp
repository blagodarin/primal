// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/dsp.hpp>

#include <algorithm>
#include <array>
#include <numeric>

#include <doctest/doctest.h>

namespace
{
	template <typename T, size_t N>
	constexpr bool checkSize(const std::array<T, N>&) noexcept
	{
		return N * sizeof(T) > primal::kDspAlignment && N * sizeof(T) % primal::kDspAlignment == 0;
	}
}

TEST_CASE("addSaturate1D")
{
	alignas(primal::kDspAlignment) std::array<float, 16> first{};
	static_assert(::checkSize(first));
	alignas(primal::kDspAlignment) const std::array<float, first.size()> second{
		-.875f, -.75f, -.625f, -.5f, -.375f, -.25f, -.125f, -0.f,
		.125f, .25f, .375f, .5f, .625f, .75f, .875f, 1.f
	};
	const std::array<float, first.size()> expected{
		-1.f, -1.f, -1.f, -1.f, -.875f, -.625f, -.375f, -.125f,
		.125f, .375f, .625f, .875f, 1.f, 1.f, 1.f, 1.f
	};
	for (auto size = first.size(); size >= first.size() - primal::kDspAlignment / sizeof first[0]; --size)
	{
		INFO("size = " << size);
		std::generate_n(first.begin(), size, [value = -1.f]() mutable {
			const auto result = value;
			value += .125f;
			return result;
		});
		std::fill_n(first.begin() + static_cast<ptrdiff_t>(size), first.size() - size, 2.f);
		primal::addSaturate1D(first.data(), second.data(), size);
		for (size_t i = 0; i < size; ++i)
		{
			INFO("i = " << i);
			CHECK(first[i] == expected[i]);
		}
		for (size_t i = size; i < first.size(); ++i)
		{
			INFO("i = " << i);
			CHECK(first[i] == 2.f);
		}
	}
}

TEST_CASE("duplicate1D_16")
{
	alignas(primal::kDspAlignment) const std::array<int16_t, 16> input{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	static_assert(::checkSize(input));
	alignas(primal::kDspAlignment) std::array<int16_t, input.size() * 2> output{};
	for (auto size = input.size(); size >= input.size() - primal::kDspAlignment / sizeof input[0]; --size)
	{
		INFO("size = " << size);
		std::fill(output.begin(), output.end(), int16_t{ 0 });
		primal::duplicate1D_16(output.data(), input.data(), size);
		for (size_t i = 0; i < size * 2; ++i)
		{
			INFO("i = " << i);
			CHECK(output[i] == input[i / 2]);
		}
		for (auto i = size * 2; i < output.size(); ++i)
		{
			INFO("i = " << i);
			CHECK(output[i] == 0);
		}
	}
}

TEST_CASE("duplicate1D_32")
{
	alignas(primal::kDspAlignment) const std::array<int32_t, 8> input{ 1, 2, 3, 4, 5, 6, 7, 8 };
	static_assert(::checkSize(input));
	alignas(primal::kDspAlignment) std::array<int32_t, input.size() * 2> output{};
	for (auto size = input.size(); size >= input.size() - primal::kDspAlignment / sizeof input[0]; --size)
	{
		INFO("size = " << size);
		std::fill(output.begin(), output.end(), int32_t{ 0 });
		primal::duplicate1D_32(output.data(), input.data(), size);
		for (size_t i = 0; i < size * 2; ++i)
		{
			INFO("i = " << i);
			CHECK(output[i] == input[i / 2]);
		}
		for (auto i = size * 2; i < output.size(); ++i)
		{
			INFO("i = " << i);
			CHECK(output[i] == 0);
		}
	}
}

TEST_CASE("normalize1D")
{
	alignas(primal::kDspAlignment) const std::array<int16_t, 16> input{
		-32768, -28672, -24576, -20480, -16384, -12288, -8192, -4096,
		0, 4096, 8192, 12288, 16384, 20480, 24576, 28672
	};
	static_assert(::checkSize(input));
	const std::array<float, input.size()> expected{
		-1.f, -.875f, -.75f, -.625f, -.5f, -.375f, -.25f, -.125f,
		0.f, .125f, .25f, .375f, .5f, .625f, .75f, .875f
	};
	alignas(primal::kDspAlignment) std::array<float, expected.size()> actual{};
	for (auto size = input.size(); size >= input.size() - primal::kDspAlignment / sizeof input[0]; --size)
	{
		INFO("size = " << size);
		std::fill(actual.begin(), actual.end(), 2.f);
		primal::normalize1D(actual.data(), input.data(), size);
		for (size_t i = 0; i < size; ++i)
		{
			INFO("i = " << i);
			CHECK(actual[i] == expected[i]);
		}
		for (size_t i = size; i < actual.size(); ++i)
		{
			INFO("i = " << i);
			CHECK(actual[i] == 2.f);
		}
	}
}

TEST_CASE("normalizeDuplicate1D")
{
	alignas(primal::kDspAlignment) const std::array<int16_t, 16> input{
		-32768, -28672, -24576, -20480, -16384, -12288, -8192, -4096,
		0, 4096, 8192, 12288, 16384, 20480, 24576, 28672
	};
	static_assert(::checkSize(input));
	const std::array<float, input.size() * 2> expected{
		-1.000f, -1.000f, -0.875f, -0.875f, -0.750f, -0.750f, -0.625f, -0.625f,
		-0.500f, -0.500f, -0.375f, -0.375f, -0.250f, -0.250f, -0.125f, -0.125f,
		0.000f, 0.000f, 0.125f, 0.125f, 0.250f, 0.250f, 0.375f, 0.375f,
		0.500f, 0.500f, 0.625f, 0.625f, 0.750f, 0.750f, 0.875f, 0.875f
	};
	alignas(primal::kDspAlignment) std::array<float, expected.size()> actual{};
	for (auto size = input.size(); size >= input.size() - primal::kDspAlignment / sizeof input[0]; --size)
	{
		INFO("size = " << size);
		std::fill(actual.begin(), actual.end(), 2.f);
		primal::normalizeDuplicate1D(actual.data(), input.data(), size);
		for (size_t i = 0; i < size * 2; ++i)
		{
			INFO("i = " << i);
			CHECK(actual[i] == expected[i]);
		}
		for (auto i = size * 2; i < actual.size(); ++i)
		{
			INFO("i = " << i);
			CHECK(actual[i] == 2.f);
		}
	}
}
