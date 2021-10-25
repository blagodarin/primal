// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string_view>

namespace primal
{
	// Checks if the wildcard pattern matches the specified text.
	// Wildcard symbols are '?' (matches any character) and '*' (matches any number of any characters).
	[[nodiscard]] constexpr bool matchWildcard(std::string_view text, std::string_view pattern) noexcept
	{
		auto t = text.begin();
		auto p = pattern.begin();
		auto textRestart = text.end();
		auto patternRestart = pattern.end();
		while (t != text.end())
		{
			if (p != pattern.end())
			{
				if (*p == '*')
				{
					textRestart = t;
					patternRestart = ++p;
					continue;
				}
				if (*p == '?' || *t == *p)
				{
					++t;
					++p;
					continue;
				}
			}
			if (textRestart == text.end())
				return false;
			t = ++textRestart;
			p = patternRestart;
		}
		for (; p != pattern.end(); ++p)
			if (*p != '*')
				return false;
		return true;
	}
}
