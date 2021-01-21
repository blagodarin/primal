# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

add_executable(primal_tests
	allocator.cpp
	buffer.cpp
	c_ptr.cpp
	compatibility.cpp
	fixed.cpp
	intrinsics.cpp
	main.cpp
	rigid_vector.cpp
	static_vector.cpp
	)
target_include_directories(primal_tests PRIVATE ${PROJECT_BINARY_DIR})
target_link_libraries(primal_tests PRIVATE primal)
set_target_properties(primal_tests PROPERTIES
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
	POSITION_INDEPENDENT_CODE OFF
	)
add_test(NAME primal COMMAND primal_tests)
