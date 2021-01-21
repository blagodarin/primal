// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/allocator.hpp>
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

	constexpr size_t kAlignment = 256;

	using AlignedAllocator = primal::AlignedAllocator<kAlignment>;

	struct AlignedAllocatorDeleter
	{
		void operator()(void* pointer) noexcept
		{
			AlignedAllocator::deallocate(pointer);
		}
	};
}

TEST_CASE("Allocator::allocate()")
{
	std::unique_ptr<void, AllocatorDeleter> pointer{ primal::Allocator::allocate(1) };
	CHECK(pointer);
	const auto misalignment = std::bit_cast<uintptr_t>(pointer.get()) % kAlignment;
	MESSAGE("Default allocator misalignment: ", misalignment, " % ", kAlignment);
	CHECK(misalignment != 0);
}

TEST_CASE("AlignedAllocator::allocate()")
{
	std::unique_ptr<void, AlignedAllocatorDeleter> pointer{ AlignedAllocator::allocate(1) };
	CHECK(pointer);
	CHECK(std::bit_cast<uintptr_t>(pointer.get()) % kAlignment == 0);
}
