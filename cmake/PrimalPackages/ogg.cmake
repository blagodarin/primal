# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

function(primal_provide_ogg _output)
	cmake_parse_arguments(_arg "" "SET_UPDATED;STATIC_RUNTIME" "" ${ARGN})
	if(_arg_SET_UPDATED)
		set(${_arg_SET_UPDATED} OFF PARENT_SCOPE)
	endif()
	if(_arg_STATIC_RUNTIME)
		set(_patch ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ogg.patch)
	else()
		set(_patch "")
	endif()
	set(_version "1.3.4")
	set(_package "libogg-${_version}")
	primal_download("http://downloads.xiph.org/releases/ogg/${_package}.tar.xz"
		SHA1 "f07499a35566aa62affb5ca989f62eed5b8092c3"
		EXTRACT_DIR ${_package}
		PATCH ${_patch}
		RESULT _downloaded)
	set(_install_dir ${PRIMAL_PACKAGE_DIR}/ogg)
	if(_downloaded OR NOT EXISTS ${_install_dir})
		set(_source_dir ${CMAKE_BINARY_DIR}/${_package})
		set(_build_dir ${_source_dir}-build)
		message(STATUS "[PRIMAL] Building Ogg from ${_source_dir}")
		_primal_cmake(${_source_dir} ${_build_dir} ${_install_dir} OPTIONS
			-DINSTALL_DOCS=OFF
			-DINSTALL_PKG_CONFIG_MODULE=OFF
			MSVC_WARNINGS 4244
			)
		message(STATUS "[PRIMAL] Provided Ogg at ${_install_dir}")
		if(_arg_SET_UPDATED)
			set(${_arg_SET_UPDATED} ON PARENT_SCOPE)
		endif()
	endif()
	set(${_output} ${_install_dir} PARENT_SCOPE)
endfunction()
