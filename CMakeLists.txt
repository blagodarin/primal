# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)

if(DEFINED PROJECT_NAME)
	set(PRIMAL_SUBPROJECT ON)
	set(CMAKE_FOLDER ".subprojects")
endif()

project(Primal LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

if(NOT PRIMAL_SUBPROJECT)
	set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ".cmake")
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

option(PRIMAL_TESTS "Build Primal tests" OFF)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_options(-Werror -Wall -Wextra)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_compile_options(-Werror -Wall -Wextra)
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

function(primal_download _url)
	cmake_parse_arguments(_arg "" "SHA1" "" ${ARGN})
	string(REGEX REPLACE "^.*/([^/]+)$" "\\1" _name ${_url})
	set(_path ${PROJECT_BINARY_DIR}/${_name})
	if(EXISTS ${_path})
		if(NOT _arg_SHA1)
			return()
		endif()
		file(SHA1 ${_path} _existing_sha1)
		if(${_arg_SHA1} STREQUAL ${_existing_sha1})
			return()
		endif()
		message(STATUS "Removing ${_name} (hash mismatch)")
		file(REMOVE ${_path})
	endif()
	message(STATUS "Downloading ${_name}")
	if(_arg_SHA1)
		file(DOWNLOAD ${_url} ${_path} TLS_VERIFY ON EXPECTED_HASH SHA1=${_arg_SHA1})
	else()
		file(DOWNLOAD ${_url} ${_path} TLS_VERIFY ON)
	endif()
endfunction()

function(primal_install _target)
	cmake_parse_arguments(arg "" "EXPORT" "" ${ARGN})
	if(arg_EXPORT)
		set(keyword_EXPORT "EXPORT")
	else()
		set(keyword_EXPORT "")
	endif()
	install(TARGETS ${_target} ${keyword_EXPORT} ${arg_EXPORT}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		get_target_property(target_type ${_target} TYPE)
		get_target_property(target_name ${_target} OUTPUT_NAME)
		get_target_property(target_postfix ${_target} DEBUG_POSTFIX)
		if(NOT target_name)
			set(target_name ${_target})
		endif()
		if(NOT target_postfix)
			set(target_postfix "")
		endif()
		if(target_type STREQUAL STATIC_LIBRARY)
			set_target_properties(${_target} PROPERTIES COMPILE_PDB_NAME "${target_name}" COMPILE_PDB_NAME_DEBUG "${target_name}${target_postfix}")
			install(FILES "$<TARGET_FILE_DIR:${_target}>/$<$<NOT:$<CONFIG:Debug>>:$<TARGET_PROPERTY:${_target},COMPILE_PDB_NAME>>$<$<CONFIG:Debug>:$<TARGET_PROPERTY:${_target},COMPILE_PDB_NAME_DEBUG>>.pdb" DESTINATION ${CMAKE_INSTALL_LIBDIR} OPTIONAL)
		elseif(NOT target_type STREQUAL INTERFACE_LIBRARY)
			set_target_properties(${_target} PROPERTIES PDB_NAME "${target_name}" PDB_NAME_DEBUG "${target_name}${target_postfix}")
			install(FILES "$<TARGET_PDB_FILE:${_target}>" DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL)
		endif()
	endif()
endfunction()

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

configure_package_config_file(PrimalConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/PrimalConfig.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Primal)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/PrimalConfigVersion.cmake VERSION 0.0.0 COMPATIBILITY AnyNewerVersion)
install(DIRECTORY primal DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/PrimalConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/PrimalConfigVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Primal)
install(EXPORT PrimalTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Primal NAMESPACE Primal::)
