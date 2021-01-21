// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

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
				[[likely]] return memory;
			throw std::bad_alloc{};
		}

		static void deallocate(void* memory) noexcept
		{
			std::free(memory);
		}
	};
}
