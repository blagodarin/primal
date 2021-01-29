// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <utility>

namespace primal
{
	template <typename T, typename Deleter>
	class SmartPtr : private Deleter
	{
	public:
		constexpr SmartPtr() noexcept = default;
		SmartPtr(const SmartPtr&) = delete;
		~SmartPtr() noexcept { Deleter::free(_pointer); }
		SmartPtr& operator=(const SmartPtr&) = delete;

		template <typename... DeleterArgs>
		constexpr explicit SmartPtr(T* pointer, DeleterArgs&&... args) noexcept
			: Deleter{ std::forward<DeleterArgs>(args)... }, _pointer{ pointer } {}

		constexpr SmartPtr(SmartPtr&& other) noexcept
			: Deleter{ static_cast<Deleter&&>(other) }, _pointer{ std::exchange(other._pointer, nullptr) } {}

		constexpr SmartPtr& operator=(SmartPtr&& other) noexcept
		{
			swap(*this, other);
			return *this;
		}

		[[nodiscard]] constexpr operator T*() const noexcept { return _pointer; }
		[[nodiscard]] constexpr T* operator->() const noexcept { return _pointer; }
		[[nodiscard]] constexpr T* get() const noexcept { return _pointer; }
		[[nodiscard]] constexpr T** out() noexcept { return &_pointer; }

		friend constexpr void swap(SmartPtr& first, SmartPtr& second) noexcept
		{
			using std::swap;
			if constexpr (sizeof(SmartPtr) > sizeof(void*))
				swap(static_cast<Deleter&>(first), static_cast<Deleter&>(second));
			swap(first._pointer, second._pointer);
		}

	private:
		T* _pointer = nullptr;
	};

	template <auto deleter>
	class FunctionDeleter
	{
	public:
		template <typename T>
		static void free(T* pointer) noexcept
		{
			if (pointer)
				deleter(pointer);
		}
	};

	// Smart pointer for working with C APIs.
	template <typename T, auto deleter>
	using CPtr = SmartPtr<T, FunctionDeleter<deleter>>;
}
