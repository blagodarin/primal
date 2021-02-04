# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

add_executable(primal_tests
	allocator.cpp
	buffer.cpp
	fixed.cpp
	intrinsics.cpp
	macros.cpp
	main.cpp
	pointer.cpp
	rigid_vector.cpp
	scope.cpp
	static_vector.cpp
	)
target_include_directories(primal_tests PRIVATE ${PROJECT_BINARY_DIR})
target_link_libraries(primal_tests PRIVATE primal)
primal_set_properties(primal_tests STANDALONE)
add_test(NAME primal COMMAND primal_tests)
