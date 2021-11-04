// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/buffer.hpp>
#include <primal/dsp.hpp>

#include <algorithm>

#include <benchmark/benchmark.h>

namespace
{
	void baselineAddSaturate1D(float* dst, const float* src, size_t length) noexcept
	{
		for (size_t i = 0; i < length; ++i)
		{
			const auto value = dst[i] + src[i];
			dst[i] = value < -1.f ? -1.f : (value > 1.f ? 1.f : value);
		}
	}

	void addSaturate1D_aligned(benchmark::State& state)
	{
		primal::Buffer<float, primal::AlignedAllocator<primal::kDspAlignment>> first{ static_cast<size_t>(state.range(0)) / sizeof(float) };
		primal::Buffer<float, primal::AlignedAllocator<primal::kDspAlignment>> second{ static_cast<size_t>(state.range(0)) / sizeof(float) };
		std::fill_n(first.data(), first.capacity(), 1.f);
		std::fill_n(second.data(), second.capacity(), 1.f);
		for (auto _ : state)
			primal::addSaturate1D(first.data(), second.data(), first.capacity());
	}

	void addSaturate1D_aligned_baseline(benchmark::State& state)
	{
		primal::Buffer<float, primal::AlignedAllocator<primal::kDspAlignment>> first{ static_cast<size_t>(state.range(0)) / sizeof(float) };
		primal::Buffer<float, primal::AlignedAllocator<primal::kDspAlignment>> second{ static_cast<size_t>(state.range(0)) / sizeof(float) };
		std::fill_n(first.data(), first.capacity(), 1.f);
		std::fill_n(second.data(), second.capacity(), 1.f);
		for (auto _ : state)
			baselineAddSaturate1D(first.data(), second.data(), first.capacity());
	}

	void addSaturate1D_unaligned(benchmark::State& state)
	{
		primal::Buffer<float, primal::AlignedAllocator<primal::kDspAlignment>> first{ static_cast<size_t>(state.range(0) - 1) / sizeof(float) };
		primal::Buffer<float, primal::AlignedAllocator<primal::kDspAlignment>> second{ static_cast<size_t>(state.range(0) - 1) / sizeof(float) };
		std::fill_n(first.data(), first.capacity(), 1.f);
		std::fill_n(second.data(), second.capacity(), 1.f);
		for (auto _ : state)
			primal::addSaturate1D(first.data(), second.data(), first.capacity());
	}

	void addSaturate1D_unaligned_baseline(benchmark::State& state)
	{
		primal::Buffer<float, primal::AlignedAllocator<primal::kDspAlignment>> first{ static_cast<size_t>(state.range(0) - 1) / sizeof(float) };
		primal::Buffer<float, primal::AlignedAllocator<primal::kDspAlignment>> second{ static_cast<size_t>(state.range(0) - 1) / sizeof(float) };
		std::fill_n(first.data(), first.capacity(), 1.f);
		std::fill_n(second.data(), second.capacity(), 1.f);
		for (auto _ : state)
			baselineAddSaturate1D(first.data(), second.data(), first.capacity());
	}
}

BENCHMARK(addSaturate1D_aligned)->Range(primal::kDspAlignment, 1 << 20);
BENCHMARK(addSaturate1D_aligned_baseline)->Range(primal::kDspAlignment, 1 << 20);
BENCHMARK(addSaturate1D_unaligned)->Range(primal::kDspAlignment, 1 << 20);
BENCHMARK(addSaturate1D_unaligned_baseline)->Range(primal::kDspAlignment, 1 << 20);
