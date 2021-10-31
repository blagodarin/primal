// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <string_view>

namespace primal
{
	// Trailing space options for string operations.
	enum class TrailingSpace
	{
		Remove, // Remove trailing space.
		Keep,   // Keep trailing space.
	};

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

	// Replaces sequences of spaces and ASCII control characters with a single space.
	// Removes leading whitespace, and optionally removes trainling whitespace.
	inline void normalizeWhitespace(std::string& string, TrailingSpace trailingSpace) noexcept
	{
		auto out = string.begin();
		char last = '\0';
		for (auto in = out; in != string.end(); ++in)
		{
			if (static_cast<unsigned char>(*in) > 0x20)
				last = *in;
			else if (static_cast<unsigned char>(last) > 0x20)
				last = ' ';
			else
				continue;
			*out++ = last;
		}
		if (trailingSpace == TrailingSpace::Remove && last == ' ')
			--out;
		string.resize(static_cast<size_t>(out - string.begin()));
	}
}
