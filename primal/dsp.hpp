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
		for (; length > 0; --length)
		{
			const auto value = *dst + *src++;
			*dst++ = value < -1.f ? -1.f : (value > 1.f ? 1.f : value);
		}
	}

	inline void duplicate1D(int16_t* dst, const int16_t* src, size_t length) noexcept
	{
		for (; length > 0; --length)
		{
			const auto value = *src++;
			*dst++ = value;
			*dst++ = value;
		}
	}

	inline void duplicate1D(float* dst, const float* src, size_t length) noexcept
	{
#if PRIMAL_INTRINSICS_SSE
		for (; length > 4; length -= 4)
		{
			const auto block = _mm_load_ps(src);
			_mm_store_ps(dst, _mm_shuffle_ps(block, block, 0b01010000));
			_mm_store_ps(dst + 4, _mm_shuffle_ps(block, block, 0b11111010));
			src += 4;
			dst += 8;
		}
#endif
		for (; length > 0; --length)
		{
			const auto value = *src++;
			*dst++ = value;
			*dst++ = value;
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
