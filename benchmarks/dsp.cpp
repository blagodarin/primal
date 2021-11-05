// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/buffer.hpp>
#include <primal/dsp.hpp>

#include <algorithm>
#include <numeric>

#include <benchmark/benchmark.h>

namespace
{
	void baseline_add1D(float* dst, const float* src, size_t length) noexcept
	{
		for (size_t i = 0; i < length; ++i)
			dst[i] += src[i];
	}

	template <auto function, bool unaligned>
	void benchmark_add1D(benchmark::State& state)
	{
		primal::Buffer<float, primal::AlignedAllocator<primal::kDspAlignment>> first{ static_cast<size_t>(state.range(0) - static_cast<int>(unaligned)) / sizeof(float) };
		primal::Buffer<float, primal::AlignedAllocator<primal::kDspAlignment>> second{ first.capacity() };
		std::fill_n(first.data(), first.capacity(), 1.f);
		std::fill_n(second.data(), second.capacity(), 1.f);
		for (auto _ : state)
			function(first.data(), second.data(), first.capacity());
	}

	void add1D_aligned(benchmark::State& state) { benchmark_add1D<primal::add1D, false>(state); }
	void add1D_aligned_baseline(benchmark::State& state) { benchmark_add1D<baseline_add1D, false>(state); }
	void add1D_unaligned(benchmark::State& state) { benchmark_add1D<primal::add1D, true>(state); }
	void add1D_unaligned_baseline(benchmark::State& state) { benchmark_add1D<baseline_add1D, true>(state); }
}

BENCHMARK(add1D_aligned)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);
BENCHMARK(add1D_aligned_baseline)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);
BENCHMARK(add1D_unaligned)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);
BENCHMARK(add1D_unaligned_baseline)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);

namespace
{
	void baseline_addNormalized1D(float* dst, const int16_t* src, size_t length) noexcept
	{
		for (size_t i = 0; i < length; ++i)
			dst[i] += static_cast<float>(src[i]) / 32768.f;
	}

	template <auto function, bool unaligned>
	void benchmark_addNormalized1D(benchmark::State& state)
	{
		primal::Buffer<int16_t, primal::AlignedAllocator<primal::kDspAlignment>> src{ static_cast<size_t>(state.range(0) - static_cast<int>(unaligned)) / sizeof(int16_t) };
		std::iota(src.data(), src.data() + src.capacity(), int16_t{ 0 });
		primal::Buffer<float, primal::AlignedAllocator<primal::kDspAlignment>> dst{ src.capacity() };
		std::iota(dst.data(), dst.data() + dst.capacity(), 1.f);
		for (auto _ : state)
			function(dst.data(), src.data(), src.capacity());
	}

	void addNormalized1D_aligned(benchmark::State& state) { benchmark_addNormalized1D<primal::addNormalized1D, false>(state); }
	void addNormalized1D_aligned_baseline(benchmark::State& state) { benchmark_addNormalized1D<baseline_addNormalized1D, false>(state); }
	void addNormalized1D_unaligned(benchmark::State& state) { benchmark_addNormalized1D<primal::addNormalized1D, true>(state); }
	void addNormalized1D_unaligned_baseline(benchmark::State& state) { benchmark_addNormalized1D<baseline_addNormalized1D, true>(state); }
}

BENCHMARK(addNormalized1D_aligned)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);
BENCHMARK(addNormalized1D_aligned_baseline)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);
BENCHMARK(addNormalized1D_unaligned)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);
BENCHMARK(addNormalized1D_unaligned_baseline)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);

namespace
{
	void baseline_addNormalizedDuplicated1D(float* dst, const int16_t* src, size_t length) noexcept
	{
		for (size_t i = 0; i < length; ++i)
		{
			const auto value = static_cast<float>(src[i]) / 32768.f;
			dst[i * 2] += value;
			dst[i * 2 + 1] += value;
		}
	}

	template <auto function, bool unaligned>
	void benchmark_addNormalizedDuplicated1D(benchmark::State& state)
	{
		primal::Buffer<int16_t, primal::AlignedAllocator<primal::kDspAlignment>> src{ static_cast<size_t>(state.range(0) - static_cast<int>(unaligned)) / sizeof(int16_t) };
		std::iota(src.data(), src.data() + src.capacity(), int16_t{ 1 });
		primal::Buffer<float, primal::AlignedAllocator<primal::kDspAlignment>> dst{ src.capacity() * 2 };
		std::iota(dst.data(), dst.data() + dst.capacity(), 1.f);
		for (auto _ : state)
			function(dst.data(), src.data(), src.capacity());
	}

	void addNormalizedDuplicated1D_aligned(benchmark::State& state) { benchmark_addNormalizedDuplicated1D<primal::addNormalizedDuplicated1D, false>(state); }
	void addNormalizedDuplicated1D_aligned_baseline(benchmark::State& state) { benchmark_addNormalizedDuplicated1D<baseline_addNormalizedDuplicated1D, false>(state); }
	void addNormalizedDuplicated1D_unaligned(benchmark::State& state) { benchmark_addNormalizedDuplicated1D<primal::addNormalizedDuplicated1D, true>(state); }
	void addNormalizedDuplicated1D_unaligned_baseline(benchmark::State& state) { benchmark_addNormalizedDuplicated1D<baseline_addNormalizedDuplicated1D, true>(state); }
}

BENCHMARK(addNormalizedDuplicated1D_aligned)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);
BENCHMARK(addNormalizedDuplicated1D_aligned_baseline)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);
BENCHMARK(addNormalizedDuplicated1D_unaligned)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);
BENCHMARK(addNormalizedDuplicated1D_unaligned_baseline)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);

namespace
{
	void baseline_addSaturate1D(float* dst, const float* src, size_t length) noexcept
	{
		for (size_t i = 0; i < length; ++i)
		{
			const auto value = dst[i] + src[i];
			dst[i] = value < -1.f ? -1.f : (value > 1.f ? 1.f : value);
		}
	}

	template <auto function, bool unaligned>
	void benchmark_addSaturate1D(benchmark::State& state)
	{
		primal::Buffer<float, primal::AlignedAllocator<primal::kDspAlignment>> first{ static_cast<size_t>(state.range(0) - static_cast<int>(unaligned)) / sizeof(float) };
		primal::Buffer<float, primal::AlignedAllocator<primal::kDspAlignment>> second{ first.capacity() };
		std::fill_n(first.data(), first.capacity(), 1.f);
		std::fill_n(second.data(), second.capacity(), 1.f);
		for (auto _ : state)
			function(first.data(), second.data(), first.capacity());
	}

	void addSaturate1D_aligned(benchmark::State& state) { benchmark_addSaturate1D<primal::addSaturate1D, true>(state); }
	void addSaturate1D_aligned_baseline(benchmark::State& state) { benchmark_addSaturate1D<baseline_addSaturate1D, true>(state); }
	void addSaturate1D_unaligned(benchmark::State& state) { benchmark_addSaturate1D<primal::addSaturate1D, false>(state); }
	void addSaturate1D_unaligned_baseline(benchmark::State& state) { benchmark_addSaturate1D<baseline_addSaturate1D, false>(state); }
}

BENCHMARK(addSaturate1D_aligned)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);
BENCHMARK(addSaturate1D_aligned_baseline)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);
BENCHMARK(addSaturate1D_unaligned)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);
BENCHMARK(addSaturate1D_unaligned_baseline)->Arg(primal::kDspAlignment)->Arg(2 * primal::kDspAlignment)->RangeMultiplier(16)->Range(1 << 8, 1 << 20);
