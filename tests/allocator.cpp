// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <primal/allocator.hpp>

#include <limits>
#include <memory>
#include <type_traits>

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

	using AllocatorPtr = std::unique_ptr<void, AllocatorDeleter>;

	constexpr size_t kAlignment = 256;

	using AlignedAllocator = primal::AlignedAllocator<kAlignment>;

	struct AlignedAllocatorDeleter
	{
		void operator()(void* pointer) noexcept
		{
			AlignedAllocator::deallocate(pointer);
		}
	};

	using AlignedAllocatorPtr = std::unique_ptr<void, AlignedAllocatorDeleter>;

	// GCC issues a warning if the allocation size is known at compile time and exceeds this value.
	constexpr auto kMaxSize = static_cast<size_t>(std::numeric_limits<std::make_signed_t<size_t>>::max());
}

TEST_CASE("Allocator::allocate(1)")
{
	AllocatorPtr pointer{ primal::Allocator::allocate(1) };
	CHECK(pointer);
	const auto misalignment = reinterpret_cast<uintptr_t>(pointer.get()) % kAlignment;
	MESSAGE("Default allocator misalignment: ", misalignment, " % ", kAlignment);
	CHECK(misalignment != 0);
}

TEST_CASE("Allocator::allocate(SIZE_MAX)")
{
	CHECK_THROWS_AS(AllocatorPtr pointer{ primal::Allocator::allocate(kMaxSize) }, std::bad_alloc);
}

TEST_CASE("AlignedAllocator::allocate(1)")
{
	AlignedAllocatorPtr pointer{ AlignedAllocator::allocate(1) };
	CHECK(pointer);
	CHECK(reinterpret_cast<uintptr_t>(pointer.get()) % kAlignment == 0);
}

TEST_CASE("AlignedAllocator::allocate(SIZE_MAX)")
{
	CHECK_THROWS_AS(AlignedAllocatorPtr pointer{ AlignedAllocator::allocate(kMaxSize - kMaxSize % kAlignment) }, std::bad_alloc);
}
