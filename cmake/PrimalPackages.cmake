# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

include(${CMAKE_CURRENT_LIST_DIR}/PrimalUtils.cmake)

set(PRIMAL_PACKAGE_DIR ${CMAKE_BINARY_DIR}/_PrimalPackages CACHE PATH "Directory for Primal packages")
mark_as_advanced(PRIMAL_PACKAGE_DIR)

function(_primal_cmake _source_dir _build_dir _install_dir)
	cmake_parse_arguments(_arg "" "TARGET" "MSVC_WARNINGS;OPTIONS" ${ARGN})
	file(REMOVE_RECURSE ${_build_dir})
	file(MAKE_DIRECTORY ${_build_dir})
	file(REMOVE_RECURSE ${_install_dir})
	set(_options ${_source_dir} -G ${CMAKE_GENERATOR})
	if(CMAKE_GENERATOR_PLATFORM)
		list(APPEND _options -A ${CMAKE_GENERATOR_PLATFORM})
	endif()
	if(CMAKE_GENERATOR_TOOLSET)
		list(APPEND _options -T ${CMAKE_GENERATOR_TOOLSET})
	endif()
	list(APPEND _options
		-DCMAKE_INSTALL_PREFIX=${_install_dir}
		-DCMAKE_POSITION_INDEPENDENT_CODE=OFF
		)
	get_property(_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
	if(_is_multi_config)
		list(APPEND _options -DCMAKE_DEBUG_POSTFIX=d)
	elseif(CMAKE_BUILD_TYPE)
		list(APPEND _options -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
	endif()
	if(_arg_TARGET)
		set(_target ${_arg_TARGET})
		if(_is_multi_config)
			list(APPEND _options -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG=${_install_dir}/lib -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE=${_install_dir}/lib)
		else()
			list(APPEND _options -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${_install_dir}/lib)
		endif()
	else()
		set(_target "install")
	endif()
	if(MSVC AND _arg_MSVC_WARNINGS)
		set(_msvc_warnings)
		foreach(_warning ${_arg_MSVC_WARNINGS})
			set(_msvc_warnings "${_msvc_warnings} /wd${_warning}")
		endforeach()
		set(_env "CFLAGS=$ENV{CFLAGS}${_msvc_warnings}")
	endif()
	execute_process(COMMAND ${CMAKE_COMMAND} -E env RCFLAGS=/nologo ${_env} ${CMAKE_COMMAND} ${_options} ${_arg_OPTIONS} WORKING_DIRECTORY ${_build_dir} COMMAND_ERROR_IS_FATAL ANY)
	if(CMAKE_GENERATOR MATCHES "^Visual Studio")
		set(_build_options -- /nologo)
	else()
		set(_build_options "")
	endif()
	if(_is_multi_config)
		execute_process(COMMAND ${CMAKE_COMMAND} --build ${_build_dir} --target ${_target} --parallel --config Debug ${_build_options} COMMAND_ERROR_IS_FATAL ANY)
		execute_process(COMMAND ${CMAKE_COMMAND} --build ${_build_dir} --target ${_target} --parallel --config Release ${_build_options} COMMAND_ERROR_IS_FATAL ANY)
	else()
		execute_process(COMMAND ${CMAKE_COMMAND} --build ${_build_dir} --target ${_target} --parallel ${_build_options} COMMAND_ERROR_IS_FATAL ANY)
	endif()
endfunction()

include(${CMAKE_CURRENT_LIST_DIR}/PrimalPackages/benchmark.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/PrimalPackages/doctest.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/PrimalPackages/fmt.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/PrimalPackages/freetype.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/PrimalPackages/jpeg.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/PrimalPackages/nasm.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/PrimalPackages/ogg.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/PrimalPackages/vorbis.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/PrimalPackages/zlib.cmake)
