# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

project(Primal LANGUAGES CXX)

include(cmake/PrimalPackages.cmake)

primal_set_defaults()

if(PROJECT_IS_TOP_LEVEL)
	option(PRIMAL_BENCHMARKS "Build Primal benchmarks" OFF)
	option(PRIMAL_TESTS "Build Primal tests" OFF)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_options(-Werror -Weverything
		-Wno-c++98-compat
		-Wno-c++98-compat-pedantic
		)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_compile_options(-Werror -Wall -Wextra
		-Wconversion
		-Wdisabled-optimization
		-Wduplicated-branches
		-Wduplicated-cond
		-Wlogical-op
		-Wmissing-declarations
		-Wmissing-include-dirs
		-Wnull-dereference
		-Wpedantic
		-Wredundant-decls
		-Wshadow
		-Wsign-conversion
		-Wundef
		-Wuninitialized
		-Wunsafe-loop-optimizations
		-Wunused-macros
		)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options(/Wall /WX
		/wd4514 # unreferenced inline function has been removed
		/wd4623 # default constructor was implicitly defined as deleted
		/wd4625 # copy constructor was implicitly defined as deleted
		/wd4626 # assignment operator was implicitly defined as deleted
		/wd4710 # function not inlined
		/wd4711 # function selected for automatic inline expansion
		/wd4738 # storing 32-bit float result in memory, possible loss of performance
		/wd4820 # 'bytes' bytes padding added after construct 'member_name'
		/wd5027 # move assignment operator was implicitly defined as deleted
		/wd5045 # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
		)
endif()

add_library(primal INTERFACE)
target_include_directories(primal INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_sources(primal PRIVATE
	primal/allocator.hpp
	primal/buffer.hpp
	primal/dsp.hpp
	primal/endian.hpp
	primal/fixed.hpp
	primal/intrinsics.hpp
	primal/macros.hpp
	primal/pointer.hpp
	primal/rigid_vector.hpp
	primal/scope.hpp
	primal/static_vector.hpp
	primal/string_utils.hpp
	primal/utf8.hpp
	)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(primal INTERFACE -msse4.1)
endif()
primal_install(primal EXPORT PrimalTargets)

if(PRIMAL_TESTS)
	primal_provide_doctest(doctest_ROOT STATIC_RUNTIME ON)
	find_package(doctest REQUIRED)
	enable_testing()
	add_subdirectory(tests)
endif()

if(PRIMAL_BENCHMARKS)
	primal_provide_benchmark(benchmark_ROOT)
	find_package(benchmark REQUIRED)
	add_subdirectory(benchmarks)
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/PrimalConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/PrimalConfig.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Primal)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/PrimalConfigVersion.cmake VERSION 0.0.0 COMPATIBILITY AnyNewerVersion ARCH_INDEPENDENT)
install(DIRECTORY primal DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/PrimalConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/PrimalConfigVersion.cmake cmake/PrimalUtils.cmake cmake/PrimalPackages.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Primal)
install(DIRECTORY cmake/PrimalPackages DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Primal)
install(EXPORT PrimalTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Primal NAMESPACE Primal::)
