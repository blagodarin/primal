// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/allocator.hpp>

#include <memory>

#include <doctest.h>

namespace
{
	struct AllocatorDeleter
	{
		void operator()(void* pointer) noexcept
		{
			primal::Allocator::deallocate(pointer);
		}
	};
}

TEST_CASE("Allocator::allocate()")
{
	std::unique_ptr<void, AllocatorDeleter> pointer{ primal::Allocator::allocate(1) };
	CHECK(pointer);
}
