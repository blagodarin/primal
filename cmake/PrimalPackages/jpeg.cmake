# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

include(${CMAKE_CURRENT_LIST_DIR}/nasm.cmake)

function(primal_provide_jpeg _output)
	cmake_parse_arguments(_arg "" "SET_UPDATED;STATIC_RUNTIME" "" ${ARGN})
	if(_arg_SET_UPDATED)
		set(${_arg_SET_UPDATED} OFF PARENT_SCOPE)
	endif()
	if(_arg_STATIC_RUNTIME)
		set(_patch ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/jpeg.patch)
	else()
		set(_patch "")
	endif()
	primal_provide_nasm(_nasm SET_UPDATED _nasm_updated)
	set(_version "2.0.6")
	set(_package "libjpeg-turbo-${_version}")
	primal_download("https://downloads.sourceforge.net/project/libjpeg-turbo/${_version}/${_package}.tar.gz"
		SHA1 "5406c7676d7df89fb4da791ad5af51202910fb25"
		EXTRACT_DIR "${_package}"
		PATCH ${_patch}
		RESULT _downloaded)
	set(_install_dir ${PRIMAL_PACKAGE_DIR}/jpeg)
	if(_downloaded OR NOT EXISTS ${_install_dir} OR _nasm_updated)
		set(_source_dir ${CMAKE_BINARY_DIR}/${_package})
		set(_build_dir ${_source_dir}-build)
		message(STATUS "[PRIMAL] Building JPEG from ${_source_dir}")
		_primal_cmake(${_source_dir} ${_build_dir} ${_install_dir} TARGET jpeg-static OPTIONS
			-DCMAKE_ASM_NASM_COMPILER=${_nasm}
			-DENABLE_SHARED=OFF
			-DREQUIRE_SIMD=ON
			-DWITH_ARITH_DEC=OFF
			-DWITH_ARITH_ENC=OFF
			-DWITH_CRT_DLL=ON # Doesn't work, set to ON to prevent manual flag manipulation.
			-DWITH_TURBOJPEG=OFF
			)
		file(INSTALL
			${_build_dir}/jconfig.h
			${_source_dir}/jerror.h
			${_source_dir}/jmorecfg.h
			${_source_dir}/jpeglib.h
			DESTINATION ${_install_dir}/include)
		message(STATUS "[PRIMAL] Provided JPEG at ${_install_dir}")
		if(_arg_SET_UPDATED)
			set(${_arg_SET_UPDATED} ON PARENT_SCOPE)
		endif()
	endif()
	set(${_output} ${_install_dir} PARENT_SCOPE)
endfunction()
