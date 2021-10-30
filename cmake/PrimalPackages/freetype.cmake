# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

function(primal_provide_freetype _output)
	cmake_parse_arguments(_arg "" "SET_UPDATED;STATIC_RUNTIME" "" ${ARGN})
	if(_arg_SET_UPDATED)
		set(${_arg_SET_UPDATED} OFF PARENT_SCOPE)
	endif()
	if(_arg_STATIC_RUNTIME)
		set(_patch ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/freetype.patch)
	else()
		set(_patch "")
	endif()
	set(_version "2.11.0")
	set(_package "freetype-${_version}")
	primal_download("https://downloads.sourceforge.net/project/freetype/freetype2/${_version}/${_package}.tar.gz"
		SHA1 "e9272ae073e35bb65aa39d55e49a309313f007a7"
		EXTRACT_DIR "${_package}"
		PATCH ${_patch}
		RESULT _downloaded)
	set(_install_dir ${PRIMAL_PACKAGE_DIR}/freetype)
	if(_downloaded OR NOT EXISTS ${_install_dir})
		set(_source_dir ${CMAKE_BINARY_DIR}/${_package})
		set(_build_dir ${_source_dir}-build)
		message(STATUS "[PRIMAL] Building Freetype from ${_source_dir}")
		_primal_cmake(${_source_dir} ${_build_dir} ${_install_dir} OPTIONS
			-DCMAKE_DISABLE_FIND_PACKAGE_BrotliDec=ON
			-DCMAKE_DISABLE_FIND_PACKAGE_BZip2=ON
			-DCMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=ON
			-DCMAKE_DISABLE_FIND_PACKAGE_PNG=ON
			-DCMAKE_DISABLE_FIND_PACKAGE_ZLIB=ON
			-DCMAKE_POLICY_DEFAULT_CMP0091=NEW # MSVC runtime library flags are selected by an abstraction.
			MSVC_WARNINGS 4244 4267 4312
			)
		message(STATUS "[PRIMAL] Provided Freetype at ${_install_dir}")
		if(_arg_SET_UPDATED)
			set(${_arg_SET_UPDATED} ON PARENT_SCOPE)
		endif()
	endif()
	set(${_output} ${_install_dir} PARENT_SCOPE)
endfunction()
