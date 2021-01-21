// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <primal/allocator.hpp>

#include <cassert>
#include <memory>

namespace primal
{
	template <typename T, typename A = Allocator>
	class Buffer
	{
	public:
		constexpr Buffer() noexcept = default;
		Buffer(const Buffer&) = delete;
		Buffer& operator=(Buffer&) = delete;

		explicit Buffer(size_t size)
			: _data{ static_cast<T*>(A::allocate(size * sizeof(T))) }
			, _size{ size }
		{
			new (_data) T[size];
		}

		~Buffer() noexcept
		{
			std::destroy_n(_data, _size);
			A::deallocate(_data);
		}

		[[nodiscard]] constexpr T* data() noexcept { return _data; }
		[[nodiscard]] constexpr const T* data() const noexcept { return _data; }
		[[nodiscard]] constexpr size_t size() const noexcept { return _size; }

		[[nodiscard]] constexpr T& operator[](size_t index) noexcept
		{
			assert(index < _size);
			return _data[index];
		}

		[[nodiscard]] constexpr const T& operator[](size_t index) const noexcept
		{
			assert(index < _size);
			return _data[index];
		}

	private:
		T* _data = nullptr;
		size_t _size = 0;
	};
}
