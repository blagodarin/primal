# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)

if(DEFINED PROJECT_NAME)
	set(PRIMAL_SUBPROJECT ON)
	set(CMAKE_FOLDER ".subprojects")
else()
	set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ".cmake")
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

project(Primal LANGUAGES CXX)

include(cmake/PrimalUtils.cmake)

option(PRIMAL_TESTS "Build Primal tests" OFF)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_options(-Werror -Weverything
		-Wno-c++98-compat
		-Wno-c++98-compat-pedantic
		-Wno-float-equal
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
	add_compile_options(/std:c++latest /Wall /WX
		/wd4514 # unreferenced inline function has been removed
		/wd4710 # function not inlined
		/wd4711 # function '___' selected for automatic inline expansion
		/wd4820 # '___' bytes padding added after data member '___'
		)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|GNU)$")
	add_compile_options(-fsanitize=address -fsanitize=undefined)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()

add_library(primal INTERFACE)
target_include_directories(primal INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.19)
	target_sources(primal PRIVATE
		primal/c_ptr.hpp
		primal/fixed.hpp
		primal/intrinsics.hpp
		primal/rigid_vector.hpp
		primal/static_vector.hpp
		)
endif()
primal_install(primal EXPORT PrimalTargets)

if(PRIMAL_TESTS)
	enable_testing()
	primal_download("https://raw.githubusercontent.com/onqtam/doctest/2.4.4/doctest/doctest.h" SHA1 "7c25b870f3e0cde6d6c8596c60f3592fe557989a")
	add_subdirectory(tests)
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/PrimalConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/PrimalConfig.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Primal)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/PrimalConfigVersion.cmake VERSION 0.0.0 COMPATIBILITY AnyNewerVersion)
install(DIRECTORY primal DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/PrimalConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/PrimalConfigVersion.cmake cmake/PrimalUtils.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Primal)
install(EXPORT PrimalTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Primal NAMESPACE Primal::)