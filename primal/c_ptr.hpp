// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

namespace primal
{
	// Smart pointer for working with C APIs.
	template <typename T, auto deleter>
	class CPtr
	{
	public:
		constexpr CPtr() noexcept = default;

		CPtr(const CPtr&) = delete;

		constexpr CPtr(CPtr&& other) noexcept
			: _pointer{ other._pointer } { other._pointer = nullptr; }

		CPtr& operator=(const CPtr&) = delete;

		constexpr CPtr& operator=(CPtr&& other) noexcept
		{
			const auto pointer = _pointer;
			_pointer = other._pointer;
			other._pointer = pointer;
			return *this;
		}

		constexpr explicit CPtr(T* pointer) noexcept
			: _pointer{ pointer } {}

		~CPtr() noexcept
		{
			if (_pointer)
				deleter(_pointer);
		}

		constexpr operator T*() const noexcept { return _pointer; }
		constexpr T* operator->() const noexcept { return _pointer; }

		constexpr auto get() const noexcept { return _pointer; }
		constexpr auto out() noexcept { return &_pointer; }

	private:
		T* _pointer = nullptr;
	};
}
