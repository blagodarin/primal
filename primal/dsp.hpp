// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <primal/intrinsics.hpp>

#include <cstddef>
#include <cstdint>

// Some DSP functions perform past-the-end memory reads if data size is not a multiple of the alignment.
// This is technically fine since there are no past-the-end writes, and reads can't access unmapped memory
// because the data is required to be aligned. However, ASAN still catches past-the-end reads, so we need
// to disable it (or write somewhat slower code).
#if PRIMAL_INTRINSICS_SSE && (defined(__GNUC__) || defined(__clang__))
#	define PRIMAL_NO_ASAN __attribute__((no_sanitize_address))
#else
#	define PRIMAL_NO_ASAN
#endif

namespace primal
{
	// Minimum alignment for DSP data.
	constexpr size_t kDspAlignment = PRIMAL_INTRINSICS_SSE ? 16 : 1;

	// Adds 32-bit floating point values, clamping the result to [-1, 1].
	// Requires SSE2.
	PRIMAL_NO_ASAN inline void addSaturate1D(float* dst, const float* src, size_t length) noexcept;

	// Duplicates 16-bit values.
	// Requires SSE2.
	inline void duplicate1D_16(void* dst, const void* src, size_t length) noexcept;

	// Duplicates 32-bit values.
	// Requires SSE2.
	inline void duplicate1D_32(void* dst, const void* src, size_t length) noexcept;

	// Converts 16-bit integers in [-32768, 32768) to 32-bit floats in [-1, 1).
	// Requires SSE4.1.
	PRIMAL_NO_ASAN inline void normalize1D(float* dst, const int16_t* src, size_t length) noexcept;

	// Converts 16-bit integers in [-32768, 32768) to 32-bit floats in [-1, 1) and duplicates every value.
	inline void normalizeDuplicate1D(float* dst, const int16_t* src, size_t length) noexcept;
}

void primal::addSaturate1D(float* dst, const float* src, size_t length) noexcept
{
	size_t i = 0;
#if PRIMAL_INTRINSICS_SSE
	for (; i < (length & ~size_t{ 0b11 }); i += 4)
		_mm_store_ps(dst + i, _mm_min_ps(_mm_set1_ps(1.f), _mm_max_ps(_mm_set1_ps(-1.f), _mm_add_ps(_mm_load_ps(dst + i), _mm_load_ps(src + i)))));
#endif
	for (; i < length; ++i)
	{
		const auto value = dst[i] + src[i];
		dst[i] = value < -1.f ? -1.f : (value > 1.f ? 1.f : value);
	}
}

void primal::duplicate1D_16(void* dst, const void* src, size_t length) noexcept
{
	size_t i = 0;
#if PRIMAL_INTRINSICS_SSE
	for (; i < (length & ~size_t{ 0b111 }); i += 8)
	{
		const auto block = _mm_load_si128(reinterpret_cast<const __m128i*>(static_cast<const uint16_t*>(src) + i));
		_mm_store_si128(reinterpret_cast<__m128i*>(static_cast<uint16_t*>(dst) + 2 * i), _mm_unpacklo_epi16(block, block));
		_mm_store_si128(reinterpret_cast<__m128i*>(static_cast<uint16_t*>(dst) + 2 * i + 8), _mm_unpackhi_epi16(block, block));
	}
#endif
	for (; i < length; ++i)
	{
		const auto value = static_cast<const uint16_t*>(src)[i]; // This does generate better assembly.
		static_cast<uint16_t*>(dst)[2 * i] = value;
		static_cast<uint16_t*>(dst)[2 * i + 1] = value;
	}
}

void primal::duplicate1D_32(void* dst, const void* src, size_t length) noexcept
{
	size_t i = 0;
#if PRIMAL_INTRINSICS_SSE
	for (; i < (length & ~size_t{ 0b11 }); i += 4)
	{
		const auto block = _mm_load_si128(reinterpret_cast<const __m128i*>(static_cast<const uint32_t*>(src) + i));
		_mm_store_si128(reinterpret_cast<__m128i*>(static_cast<uint32_t*>(dst) + 2 * i), _mm_unpacklo_epi32(block, block));
		_mm_store_si128(reinterpret_cast<__m128i*>(static_cast<uint32_t*>(dst) + 2 * i + 4), _mm_unpackhi_epi32(block, block));
	}
#endif
	for (; i < length; ++i)
	{
		const auto value = static_cast<const uint32_t*>(src)[i]; // This does generate better assembly.
		static_cast<uint32_t*>(dst)[2 * i] = value;
		static_cast<uint32_t*>(dst)[2 * i + 1] = value;
	}
}

void primal::normalize1D(float* dst, const int16_t* src, size_t length) noexcept
{
	constexpr auto unit = 1.f / 32768.f;
#if PRIMAL_INTRINSICS_SSE
	size_t i = 0;
	for (; i < (length & ~size_t{ 0b111 }); i += 8)
	{
		const auto block = _mm_load_si128(reinterpret_cast<const __m128i*>(src + i));
		_mm_store_ps(dst + i, _mm_mul_ps(_mm_set1_ps(unit), _mm_cvtepi32_ps(_mm_cvtepi16_epi32(block))));
		_mm_store_ps(dst + i + 4, _mm_mul_ps(_mm_set1_ps(unit), _mm_cvtepi32_ps(_mm_cvtepi16_epi32(_mm_srli_si128(block, 8)))));
	}
	if (length & 0b111)
	{
		const auto input = _mm_load_si128(reinterpret_cast<const __m128i*>(src + i));
		auto output = _mm_mul_ps(_mm_set1_ps(unit), _mm_cvtepi32_ps(_mm_cvtepi16_epi32(input)));
		const auto remainder = length & 0b11;
		if (length & 0b100)
		{
			_mm_store_ps(dst + i, output);
			if (!remainder)
				return;
			output = _mm_mul_ps(_mm_set1_ps(unit), _mm_cvtepi32_ps(_mm_cvtepi16_epi32(_mm_srli_si128(input, 8))));
			i += 4;
		}
		const auto mask = _mm_set_epi64x((int64_t{ 1 } << (remainder * 16)) - 1, 0);
		_mm_maskmoveu_si128(_mm_castps_si128(output), _mm_unpackhi_epi8(mask, mask), reinterpret_cast<char*>(dst + i));
	}
#else
	for (size_t i = 0; i < length; ++i)
		dst[i] = static_cast<float>(src[i]) * unit;
#endif
}

void primal::normalizeDuplicate1D(float* dst, const int16_t* src, size_t length) noexcept
{
	constexpr auto scale = 1.f / 32768.f;
	for (; length > 0; --length)
	{
		const auto value = static_cast<float>(*src++) * scale;
		*dst++ = value;
		*dst++ = value;
	}
}
