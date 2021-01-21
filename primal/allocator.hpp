// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/compatibility.hpp>

#include <bit>
#include <cstdlib>
#include <new>

namespace primal
{
	class Allocator
	{
	public:
		static void* allocate(size_t size)
		{
			if (const auto memory = std::malloc(size); memory)
				PRIMAL_LIKELY return memory;
			throw std::bad_alloc{};
		}

		static void deallocate(void* memory) noexcept
		{
			std::free(memory);
		}
	};

	template <size_t kAlignment>
	class AlignedAllocator
	{
	public:
		static_assert(std::has_single_bit(kAlignment));

		static void* allocate(size_t size)
		{
			constexpr auto kAlignmentMask = kAlignment - 1;
			const auto alignedSize = (size + kAlignmentMask) & ~kAlignmentMask;
#ifdef _MSC_VER
			if (const auto memory = ::_aligned_malloc(alignedSize, kAlignment); memory)
#else
			if (const auto memory = std::aligned_alloc(kAlignment, alignedSize); memory)
#endif
				PRIMAL_LIKELY return memory;
			throw std::bad_alloc{};
		}

		static void deallocate(void* memory) noexcept
		{
#ifdef _MSC_VER
			::_aligned_free(memory);
#else
			std::free(memory);
#endif
		}
	};
}
