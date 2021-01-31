// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <primal/macros.hpp>

#include <utility>

namespace primal
{
	template <typename Callback>
	class Finally
	{
	public:
		Finally(const Finally&) = delete;
		Finally& operator=(const Finally&) = delete;

		constexpr Finally(Callback&& callback) noexcept
			: _callback{ std::move(callback) } {}

		~Finally() noexcept { _callback(); }

	private:
		const Callback _callback;
	};

	template <typename Callback>
	auto makeFinally(Callback&& callback) noexcept
	{
		return Finally<Callback>{ std::forward<Callback>(callback) };
	}
}

#define PRIMAL_FINALLY(callback) const auto PRIMAL_JOIN(primalFinally, __LINE__) = primal::makeFinally(callback)
