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

	// Adds 32-bit floats to the output buffer with the same number of interleaved channels.
	inline void addSamples1D(float* dst, const float* src, size_t length) noexcept;

	// Converts 16-bit integers in [-32768, 32768) to 32-bit floats in [-1, 1)
	// and adds them to the output buffer with the same number of interleaved channels.
	inline void addSamples1D(float* dst, const int16_t* src, size_t length) noexcept;

	// Adds 32-bit floats to the output buffer with twice the number of interleaved channels.
	inline void addSamples2x1D(float* dst, const float* src, size_t length) noexcept;

	// Converts 16-bit integers in [-32768, 32768) to 32-bit floats in [-1, 1)
	// and adds them to the output buffer with twice the number of interleaved channels.
	inline void addSamples2x1D(float* dst, const int16_t* src, size_t length) noexcept;

	// Duplicates 16-bit values.
	inline void duplicate1D_16(void* dst, const void* src, size_t length) noexcept;

	// Duplicates 32-bit values.
	inline void duplicate1D_32(void* dst, const void* src, size_t length) noexcept;
}

void primal::addSamples1D(float* dst, const float* src, size_t length) noexcept
{
	// No manual SSE optimization succeeded.
	for (size_t i = 0; i < length; ++i)
		dst[i] += src[i];
}

void primal::addSamples1D(float* dst, const int16_t* src, size_t length) noexcept
{
	constexpr auto unit = 1.f / 32768.f;
#if PRIMAL_INTRINSICS_SSE // 10-20% faster with MSVC.
	for (; length >= 8; length -= 8)
	{
		const auto input = _mm_load_si128(reinterpret_cast<const __m128i*>(src));
		src += 8;
		_mm_store_ps(dst, _mm_add_ps(_mm_load_ps(dst), _mm_mul_ps(_mm_set1_ps(unit), _mm_cvtepi32_ps(_mm_cvtepi16_epi32(input)))));
		dst += 4;
		_mm_store_ps(dst, _mm_add_ps(_mm_load_ps(dst), _mm_mul_ps(_mm_set1_ps(unit), _mm_cvtepi32_ps(_mm_cvtepi16_epi32(_mm_srli_si128(input, 8))))));
		dst += 4;
	}
	for (; length > 0; --length) // For some reason it's faster than i-based loop with the preceding SSE-optimized loop, but slower without one.
		*dst++ += static_cast<float>(*src++) * unit;
#else
	for (size_t i = 0; i < length; ++i)
		dst[i] += static_cast<float>(src[i]) * unit;
#endif
}

void primal::addSamples2x1D(float* dst, const float* src, size_t length) noexcept
{
#if PRIMAL_INTRINSICS_SSE // 150-200% faster with MSVC.
	for (; length >= 4; length -= 4)
	{
		const auto input = _mm_load_ps(src);
		src += 4;
		_mm_store_ps(dst, _mm_add_ps(_mm_load_ps(dst), _mm_unpacklo_ps(input, input)));
		dst += 4;
		_mm_store_ps(dst, _mm_add_ps(_mm_load_ps(dst), _mm_unpackhi_ps(input, input)));
		dst += 4;
	}
#endif
	for (; length > 0; --length)
	{
		const auto value = *src++;
		*dst++ += value;
		*dst++ += value;
	}
}

void primal::addSamples2x1D(float* dst, const int16_t* src, size_t length) noexcept
{
	constexpr auto unit = 1.f / 32768.f;
#if PRIMAL_INTRINSICS_SSE // 150-170% faster with MSVC.
	for (; length >= 8; length -= 8)
	{
		const auto input = _mm_load_si128(reinterpret_cast<const __m128i*>(src));
		src += 8;
		const auto normalized1 = _mm_mul_ps(_mm_set1_ps(unit), _mm_cvtepi32_ps(_mm_cvtepi16_epi32(input)));
		const auto normalized2 = _mm_mul_ps(_mm_set1_ps(unit), _mm_cvtepi32_ps(_mm_cvtepi16_epi32(_mm_srli_si128(input, 8))));
		_mm_store_ps(dst, _mm_add_ps(_mm_load_ps(dst), _mm_unpacklo_ps(normalized1, normalized1)));
		dst += 4;
		_mm_store_ps(dst, _mm_add_ps(_mm_load_ps(dst), _mm_unpackhi_ps(normalized1, normalized1)));
		dst += 4;
		_mm_store_ps(dst, _mm_add_ps(_mm_load_ps(dst), _mm_unpacklo_ps(normalized2, normalized2)));
		dst += 4;
		_mm_store_ps(dst, _mm_add_ps(_mm_load_ps(dst), _mm_unpackhi_ps(normalized2, normalized2)));
		dst += 4;
	}
#endif
	for (; length > 0; --length)
	{
		const auto value = static_cast<float>(*src++) * unit;
		*dst++ += value;
		*dst++ += value;
	}
}

void primal::duplicate1D_16(void* dst, const void* src, size_t length) noexcept
{
	size_t i = 0;
#if PRIMAL_INTRINSICS_SSE // 4-8x faster with MSVC.
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
#if PRIMAL_INTRINSICS_SSE // 2-4x faster with MSVC.
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
