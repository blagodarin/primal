// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <primal/intrinsics.hpp>

#include <cstddef>
#include <cstdint>

namespace primal
{
	constexpr size_t kDspAlignment = PRIMAL_INTRINSICS_SSE ? 16 : 1;

	inline void addSaturate1D(float* dst, const float* src, size_t length) noexcept
	{
#if PRIMAL_INTRINSICS_SSE
		size_t i = 0;
		for (; i < (length & ~size_t{ 0b11 }); i += 4)
			_mm_store_ps(dst + i, _mm_min_ps(_mm_set1_ps(1.f), _mm_max_ps(_mm_set1_ps(-1.f), _mm_add_ps(_mm_load_ps(dst + i), _mm_load_ps(src + i)))));
		if (i != length)
		{
			const auto block = _mm_min_ps(_mm_set1_ps(1.f), _mm_max_ps(_mm_set1_ps(-1.f), _mm_add_ps(_mm_load_ps(dst + i), _mm_load_ps(src + i))));
			if (length & 2)
			{
				_mm_storel_pi(reinterpret_cast<__m64*>(dst + i), block);
				i += 2;
			}
			if (length & 1)
				_mm_store_ss(dst + i, _mm_unpackhi_ps(block, block));
		}
#else
		for (size_t i = 0; i < length; ++i)
		{
			const auto value = dst[i] + src[i];
			dst[i] = value < -1.f ? -1.f : (value > 1.f ? 1.f : value);
		}
#endif
	}

	inline void duplicate1D_16(void* dst, const void* src, size_t length) noexcept
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

	inline void duplicate1D_32(void* dst, const void* src, size_t length) noexcept
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

	inline void normalize1D(float* dst, const int16_t* src, size_t length) noexcept
	{
		constexpr auto unit = 1.f / 32768.f;
		for (; length > 0; --length)
			*dst++ = static_cast<float>(*src++) * unit;
	}

	inline void normalizeDuplicate1D(float* dst, const int16_t* src, size_t length) noexcept
	{
		constexpr auto scale = 1.f / 32768.f;
		for (; length > 0; --length)
		{
			const auto value = static_cast<float>(*src++) * scale;
			*dst++ = value;
			*dst++ = value;
		}
	}
}
