# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

function(primal_provide_benchmark _output)
	cmake_parse_arguments(_arg "" "SET_UPDATED" "" ${ARGN})
	if(_arg_SET_UPDATED)
		set(${_arg_SET_UPDATED} OFF PARENT_SCOPE)
	endif()
	set(_version "1.6.0")
	set(_package "benchmark-${_version}")
	primal_download("https://github.com/google/benchmark/archive/refs/tags/v${_version}.zip"
		SHA1 "26de5f5c784a4cdfaff7899fa2ba2a3d1b02fdf7"
		EXTRACT_DIR "${_package}"
		RESULT _downloaded)
	set(_install_dir ${PRIMAL_PACKAGE_DIR}/benchmark)
	if(_downloaded OR NOT EXISTS ${_install_dir})
		set(_source_dir ${CMAKE_BINARY_DIR}/${_package})
		set(_build_dir ${_source_dir}-build)
		message(STATUS "[PRIMAL] Building benchmark from ${_source_dir}")
		_primal_cmake(${_source_dir} ${_build_dir} ${_install_dir} OPTIONS
			-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
			-DBENCHMARK_ENABLE_GTEST_TESTS=OFF
			-DBENCHMARK_ENABLE_TESTING=OFF
			)
		message(STATUS "[PRIMAL] Provided benchmark at ${_install_dir}")
		if(_arg_SET_UPDATED)
			set(${_arg_SET_UPDATED} ON PARENT_SCOPE)
		endif()
	endif()
	set(${_output} ${_install_dir} PARENT_SCOPE)
endfunction()
