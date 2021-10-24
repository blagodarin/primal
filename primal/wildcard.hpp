// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string_view>

namespace primal
{
	[[nodiscard]] constexpr bool matchWildcard(std::string_view text, std::string_view wildcard) noexcept
	{
		auto t = text.begin();
		auto w = wildcard.begin();
		auto tRestart = text.end();
		auto wRestart = wildcard.end();
		while (t != text.end())
		{
			if (w != wildcard.end())
			{
				if (*w == '*')
				{
					++w;
					tRestart = t;
					wRestart = w;
					continue;
				}
				if (*w == '?' || *w == *t)
				{
					++w;
					++t;
					continue;
				}
			}
			if (tRestart != text.end())
			{
				++tRestart;
				t = tRestart;
				w = wRestart;
				continue;
			}
			return false;
		}
		for (; w != wildcard.end(); ++w)
			if (*w != '*')
				return false;
		return true;
	}
}
