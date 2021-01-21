// This file is part of the Primal library.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

// [[likely]] and [[unlikely]] require Clang 12.
#if __has_cpp_attribute(likely) && __has_cpp_attribute(unlikely)
#	define PRIMAL_LIKELY [[likely]]
#	define PRIMAL_UNLIKELY [[unlikely]]
#else
#	define PRIMAL_LIKELY
#	define PRIMAL_UNLIKELY
#endif
