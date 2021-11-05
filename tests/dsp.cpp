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
	constexpr auto sentinelFloat = 4.f;

	template <typename T, size_t N>
	constexpr bool checkSize(const std::array<T, N>&) noexcept
	{
		return N * sizeof(T) > 2 * primal::kDspAlignment           // To be able to check every possible remainder length.
			&& N * sizeof(T) % primal::kDspAlignment == sizeof(T); // To check we're not triggering ASAN.
	}
}

TEST_CASE("add1D")
{
	alignas(primal::kDspAlignment) std::array<float, 17> first{};
	static_assert(::checkSize(first));
	alignas(primal::kDspAlignment) const std::array<float, first.size()> second{
		-1.f, -.875f, -.75f, -.625f, -.5f, -.375f, -.25f, -.125f,
		0.f, .125f, .25f, .375f, .5f, .625f, .75f, .875f, 1.f
	};
	const std::array<float, first.size()> expected{
		-1.875f, -1.625f, -1.375f, -1.125f, -.875f, -.625f, -.375f, -.125f,
		.125f, .375f, .625f, .875f, 1.125f, 1.375f, 1.625f, 1.875f, 2.125f
	};
	for (auto size = first.size(); size >= first.size() - primal::kDspAlignment / sizeof first[0]; --size)
	{
		INFO("size = " << size);
		std::generate_n(first.begin(), size, [value = -1.f]() mutable { return value += .125f; });
		std::fill_n(first.begin() + static_cast<ptrdiff_t>(size), first.size() - size, sentinelFloat);
		primal::add1D(first.data(), second.data(), size);
		for (size_t i = 0; i < size; ++i)
		{
			INFO("i = " << i);
			CHECK(first[i] == expected[i]);
		}
		for (size_t i = size; i < first.size(); ++i)
		{
			INFO("i = " << i);
			CHECK(first[i] == sentinelFloat);
		}
	}
}

TEST_CASE("addNormalized1D")
{
	alignas(primal::kDspAlignment) std::array<float, 17> first{};
	static_assert(::checkSize(first));
	alignas(primal::kDspAlignment) const std::array<int16_t, 17> second{
		-32768, -28672, -24576, -20480, -16384, -12288, -8192, -4096,
		0, 4096, 8192, 12288, 16384, 20480, 24576, 28672, 30720
	};
	static_assert(::checkSize(second));
	const std::array<float, first.size()> expected{
		-1.875f, -1.625f, -1.375f, -1.125f, -.875f, -.625f, -.375f, -.125f,
		.125f, .375f, .625f, .875f, 1.125f, 1.375f, 1.625f, 1.875f, 2.0625f
	};
	for (auto size = first.size(); size >= first.size() - primal::kDspAlignment / sizeof first[0]; --size)
	{
		INFO("size = " << size);
		std::generate_n(first.begin(), size, [value = -1.f]() mutable { return value += .125f; });
		std::fill_n(first.begin() + static_cast<ptrdiff_t>(size), first.size() - size, sentinelFloat);
		primal::addNormalized1D(first.data(), second.data(), size);
		for (size_t i = 0; i < size; ++i)
		{
			INFO("i = " << i);
			CHECK(first[i] == expected[i]);
		}
		for (size_t i = size; i < first.size(); ++i)
		{
			INFO("i = " << i);
			CHECK(first[i] == sentinelFloat);
		}
	}
}

TEST_CASE("addNormalizedDuplicated1D")
{
	alignas(primal::kDspAlignment) const std::array<int16_t, 17> src{
		-32768, -28672, -24576, -20480, -16384, -12288, -8192, -4096,
		0, 4096, 8192, 12288, 16384, 20480, 24576, 28672, 30720
	};
	static_assert(::checkSize(src));
	const std::array<float, src.size() * 2> expected{
		-1.875f, -1.875f, -1.625f, -1.625f, -1.375f, -1.375f, -1.125f, -1.125f,
		-.875f, -.875f, -.625f, -.625f, -.375f, -.375f, -.125f, -.125f,
		.125f, .125f, .375f, .375f, .625f, .625f, .875f, .875f, 1.125f,
		1.125f, 1.375f, 1.375f, 1.625f, 1.625f, 1.875f, 1.875f, 2.0625f, 2.0625f
	};
	alignas(primal::kDspAlignment) std::array<float, expected.size()> dst{};
	for (auto size = src.size(); size >= src.size() - primal::kDspAlignment / sizeof src[0]; --size)
	{
		INFO("size = " << size);
		std::generate_n(dst.begin(), size * 2, [value = -1.f, increment = true]() mutable {
			if (increment)
				value += .125f;
			increment = !increment;
			return value;
		});
		std::fill_n(dst.begin() + static_cast<ptrdiff_t>(size * 2), dst.size() - size * 2, sentinelFloat);
		primal::addNormalizedDuplicated1D(dst.data(), src.data(), size);
		for (size_t i = 0; i < size * 2; ++i)
		{
			INFO("i = " << i);
			CHECK(dst[i] == expected[i]);
		}
		for (size_t i = size * 2; i < dst.size(); ++i)
		{
			INFO("i = " << i);
			CHECK(dst[i] == sentinelFloat);
		}
	}
}

TEST_CASE("addSaturate1D")
{
	alignas(primal::kDspAlignment) std::array<float, 17> first{};
	static_assert(::checkSize(first));
	alignas(primal::kDspAlignment) const std::array<float, first.size()> second{
		-1.f, -.875f, -.75f, -.625f, -.5f, -.375f, -.25f, -.125f,
		0.f, .125f, .25f, .375f, .5f, .625f, .75f, .875f, 1.f
	};
	const std::array<float, first.size()> expected{
		-1.f, -1.f, -1.f, -1.f, -.875f, -.625f, -.375f, -.125f,
		.125f, .375f, .625f, .875f, 1.f, 1.f, 1.f, 1.f, 1.f
	};
	for (auto size = first.size(); size >= first.size() - primal::kDspAlignment / sizeof first[0]; --size)
	{
		INFO("size = " << size);
		std::generate_n(first.begin(), size, [value = -1.f]() mutable { return value += .125f; });
		std::fill_n(first.begin() + static_cast<ptrdiff_t>(size), first.size() - size, sentinelFloat);
		primal::addSaturate1D(first.data(), second.data(), size);
		for (size_t i = 0; i < size; ++i)
		{
			INFO("i = " << i);
			CHECK(first[i] == expected[i]);
		}
		for (size_t i = size; i < first.size(); ++i)
		{
			INFO("i = " << i);
			CHECK(first[i] == sentinelFloat);
		}
	}
}

TEST_CASE("duplicate1D_16")
{
	alignas(primal::kDspAlignment) const std::array<int16_t, 17> input{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
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
	alignas(primal::kDspAlignment) const std::array<int32_t, 9> input{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
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
	alignas(primal::kDspAlignment) const std::array<int16_t, 17> input{
		-32768, -28672, -24576, -20480, -16384, -12288, -8192, -4096,
		0, 4096, 8192, 12288, 16384, 20480, 24576, 28672, 30720
	};
	static_assert(::checkSize(input));
	const std::array<float, input.size()> expected{
		-1.f, -.875f, -.75f, -.625f, -.5f, -.375f, -.25f, -.125f,
		0.f, .125f, .25f, .375f, .5f, .625f, .75f, .875f, .9375f
	};
	alignas(primal::kDspAlignment) std::array<float, expected.size()> actual{};
	for (auto size = input.size(); size >= input.size() - primal::kDspAlignment / sizeof input[0]; --size)
	{
		INFO("size = " << size);
		std::fill(actual.begin(), actual.end(), sentinelFloat);
		primal::normalize1D(actual.data(), input.data(), size);
		for (size_t i = 0; i < size; ++i)
		{
			INFO("i = " << i);
			CHECK(actual[i] == expected[i]);
		}
		for (size_t i = size; i < actual.size(); ++i)
		{
			INFO("i = " << i);
			CHECK(actual[i] == sentinelFloat);
		}
	}
}

TEST_CASE("normalizeDuplicate1D")
{
	alignas(primal::kDspAlignment) const std::array<int16_t, 17> input{
		-32768, -28672, -24576, -20480, -16384, -12288, -8192, -4096,
		0, 4096, 8192, 12288, 16384, 20480, 24576, 28672, 30720
	};
	static_assert(::checkSize(input));
	const std::array<float, input.size() * 2> expected{
		-1.f, -1.f, -.875f, -.875f, -.75f, -.75f, -.625f, -.625f,
		-.5f, -.5f, -.375f, -.375f, -.25f, -.25f, -.125f, -.125f,
		0.f, 0.f, .125f, .125f, .25f, .25f, .375f, .375f, .5f, .5f,
		.625f, .625f, .75f, .75f, .875f, .875f, .9375f, .9375f
	};
	alignas(primal::kDspAlignment) std::array<float, expected.size()> actual{};
	for (auto size = input.size(); size >= input.size() - primal::kDspAlignment / sizeof input[0]; --size)
	{
		INFO("size = " << size);
		std::fill(actual.begin(), actual.end(), sentinelFloat);
		primal::normalizeDuplicate1D(actual.data(), input.data(), size);
		for (size_t i = 0; i < size * 2; ++i)
		{
			INFO("i = " << i);
			CHECK(actual[i] == expected[i]);
		}
		for (auto i = size * 2; i < actual.size(); ++i)
		{
			INFO("i = " << i);
			CHECK(actual[i] == sentinelFloat);
		}
	}
}
