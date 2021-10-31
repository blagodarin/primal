# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

add_executable(primal_tests
	allocator.cpp
	buffer.cpp
	endian.cpp
	fixed.cpp
	intrinsics.cpp
	macros.cpp
	pointer.cpp
	rigid_vector.cpp
	scope.cpp
	static_vector.cpp
	string_utils.cpp
	utf8.cpp
	)
target_include_directories(primal_tests PRIVATE ${PRIMAL_DOWNLOAD_DIR})
target_link_libraries(primal_tests PRIVATE primal doctest::doctest_with_main)
set_target_properties(primal_tests PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_test(NAME primal COMMAND primal_tests)
