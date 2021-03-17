# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

function(primal_provide_fmt _output)
	cmake_parse_arguments(_arg "" "SET_UPDATED;STATIC_RUNTIME" "" ${ARGN})
	if(_arg_SET_UPDATED)
		set(${_arg_SET_UPDATED} OFF PARENT_SCOPE)
	endif()
	if(_arg_STATIC_RUNTIME)
		set(_patch ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/fmt.patch)
	else()
		set(_patch "")
	endif()
	set(_version "7.1.3")
	set(_package "fmt-${_version}")
	primal_download("https://github.com/fmtlib/fmt/releases/download/7.1.3/fmt-7.1.3.zip"
		SHA1 "883eb4c160b906217c859d4cd882564edbd0b24d"
		EXTRACT_DIR "${_package}"
		PATCH ${_patch}
		RESULT _downloaded)
	set(_install_dir ${PRIMAL_PACKAGE_DIR}/fmt)
	if(_downloaded OR NOT EXISTS ${_install_dir})
		set(_source_dir ${CMAKE_BINARY_DIR}/${_package})
		set(_build_dir ${_source_dir}-build)
		message(STATUS "[PRIMAL] Building fmt from ${_source_dir}")
		_primal_cmake(${_source_dir} ${_build_dir} ${_install_dir} OPTIONS
			-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
			-DFMT_DOC=OFF
			-DFMT_OS=OFF
			-DFMT_TEST=OFF
			)
		message(STATUS "[PRIMAL] Provided fmt at ${_install_dir}")
		if(_arg_SET_UPDATED)
			set(${_arg_SET_UPDATED} ON PARENT_SCOPE)
		endif()
	endif()
	set(${_output} ${_install_dir} PARENT_SCOPE)
endfunction()
