// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <primal/allocator.hpp>
#include <primal/c_ptr.hpp>

#include <cstring>
#include <memory>
#include <type_traits>

namespace primal
{
	template <typename T, typename A = Allocator>
	class Buffer
	{
	public:
		static_assert(std::is_trivially_default_constructible_v<T> && std::is_trivially_copyable_v<T> && std::is_trivially_destructible_v<T>);

		constexpr Buffer() noexcept = default;
		~Buffer() noexcept = default;
		Buffer(const Buffer&) = delete;
		Buffer& operator=(Buffer&) = delete;

		explicit Buffer(size_t size)
			: _data{ static_cast<T*>(A::allocate(size * sizeof(T))) }, _size{ size } {}

		constexpr Buffer(Buffer&& other) noexcept
			: _data{ std::move(other._data) }, _size{ other._size } { other._size = 0; }

		constexpr Buffer& operator=(Buffer&& other) noexcept
		{
			swap(*this, other);
			return *this;
		}

		[[nodiscard]] constexpr T* data() noexcept { return _data; }
		[[nodiscard]] constexpr const T* data() const noexcept { return _data; }
		[[nodiscard]] constexpr size_t size() const noexcept { return _size; }

		void reallocate(size_t newSize, bool copy = true)
		{
			decltype(_data) newData{ static_cast<T*>(A::allocate(newSize * sizeof(T))) };
			if (copy && newData && _data) // UBSan requires checking data pointers even if there is nothing to copy.
				std::memcpy(newData, _data, (newSize < _size ? newSize : _size) * sizeof(T));
			_data = std::move(newData);
			_size = newSize;
		}

		friend constexpr void swap(Buffer& first, Buffer& second) noexcept
		{
			using std::swap;
			swap(first._data, second._data);
			swap(first._size, second._size);
		}

	private:
		CPtr<T, A::deallocate> _data;
		size_t _size = 0;
	};
}
