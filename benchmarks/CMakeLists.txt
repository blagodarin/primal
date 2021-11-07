# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

add_executable(primal_benchmarks
	dsp.cpp
	)
target_link_libraries(primal_benchmarks PRIVATE primal benchmark::benchmark_main)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(primal_benchmarks PRIVATE -Wno-global-constructors)
endif()
