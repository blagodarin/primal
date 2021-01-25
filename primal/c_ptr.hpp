// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <utility>

namespace primal
{
	// Smart pointer for working with C APIs.
	template <typename T, auto deleter>
	class CPtr
	{
	public:
		constexpr CPtr() noexcept = default;
		CPtr(const CPtr&) = delete;
		CPtr& operator=(const CPtr&) = delete;

		constexpr explicit CPtr(T* pointer) noexcept
			: _pointer{ pointer } {}

		constexpr CPtr(CPtr&& other) noexcept
			: _pointer{ other._pointer } { other._pointer = nullptr; }

		constexpr CPtr& operator=(CPtr&& other) noexcept
		{
			swap(*this, other);
			return *this;
		}

		~CPtr() noexcept
		{
			if (_pointer)
				deleter(_pointer);
		}

		[[nodiscard]] constexpr operator T*() const noexcept { return _pointer; }
		[[nodiscard]] constexpr T* operator->() const noexcept { return _pointer; }
		[[nodiscard]] constexpr T* get() const noexcept { return _pointer; }
		[[nodiscard]] constexpr T** out() noexcept { return &_pointer; }

		friend constexpr void swap(CPtr& first, CPtr& second) noexcept
		{
			using std::swap;
			swap(first._pointer, second._pointer);
		}

	private:
		T* _pointer = nullptr;
	};
}
