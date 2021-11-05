# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

include(GNUInstallDirs)

set(PRIMAL_DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/_PrimalDownloads CACHE PATH "Directory for Primal downloads")
mark_as_advanced(PRIMAL_DOWNLOAD_DIR)

function(primal_download _url)
	cmake_parse_arguments(_arg "" "EXTRACT_DIR;NAME;PATCH;RESULT;SHA1" "" ${ARGN})
	if(_arg_RESULT)
		set(${_arg_RESULT} OFF PARENT_SCOPE)
	endif()
	if(_arg_NAME)
		set(_name ${_arg_NAME})
	else()
		string(REGEX REPLACE "^.*/([^/]+)$" "\\1" _name ${_url})
	endif()
	set(_path ${PRIMAL_DOWNLOAD_DIR}/${_name})
	set(_do_download ON)
	if(EXISTS ${_path})
		if(_arg_SHA1)
			file(SHA1 ${_path} _existing_sha1)
			if(NOT ${_arg_SHA1} STREQUAL ${_existing_sha1})
				message(STATUS "[PRIMAL] Removing ${_name} (hash mismatch)")
				file(REMOVE ${_path})
			else()
				set(_do_download OFF)
			endif()
		else()
			set(_do_download OFF)
		endif()
	endif()
	if(_do_download)
		message(STATUS "[PRIMAL] Downloading ${_name}")
		if(_arg_SHA1)
			file(DOWNLOAD ${_url} ${_path} STATUS _download_status TLS_VERIFY ON EXPECTED_HASH SHA1=${_arg_SHA1})
		else()
			file(DOWNLOAD ${_url} ${_path} STATUS _download_status TLS_VERIFY ON)
		endif()
		list(GET _download_status 0 _download_status_code)
		if(NOT _download_status_code EQUAL 0)
			return()
		endif()
	endif()
	if(_arg_EXTRACT_DIR)
		set(_extract_dir ${CMAKE_BINARY_DIR}/${_arg_EXTRACT_DIR})
		set(_do_extract ON)
		if(EXISTS ${_extract_dir})
			if(_do_download)
				message(STATUS "[PRIMAL] Removing ${_arg_EXTRACT_DIR}")
				file(REMOVE_RECURSE ${_extract_dir})
			else()
				set(_do_extract OFF)
			endif()
		endif()
		if(_do_extract)
			message(STATUS "[PRIMAL] Extracting ${_name}")
			execute_process(COMMAND ${CMAKE_COMMAND} -E tar x ${_path}
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
				COMMAND_ERROR_IS_FATAL ANY)
			if(_arg_PATCH)
				find_package(Git REQUIRED)
				message(STATUS "[PRIMAL] Patching ${_arg_EXTRACT_DIR}")
				execute_process(COMMAND ${GIT_EXECUTABLE} apply ${_arg_PATCH}
					WORKING_DIRECTORY ${_extract_dir}
					COMMAND_ERROR_IS_FATAL ANY)
			endif()
		endif()
	endif()
	if(_arg_RESULT AND (_do_download OR _do_extract))
		set(${_arg_RESULT} ON PARENT_SCOPE)
	endif()
endfunction()

function(primal_install _target)
	cmake_parse_arguments(_arg "" "EXPORT" "" ${ARGN})
	if(_arg_EXPORT)
		set(_keyword_EXPORT "EXPORT")
	else()
		set(_keyword_EXPORT "")
	endif()
	install(TARGETS ${_target} ${_keyword_EXPORT} ${_arg_EXPORT}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		get_target_property(_target_type ${_target} TYPE)
		get_target_property(_target_name ${_target} OUTPUT_NAME)
		get_target_property(_target_postfix ${_target} DEBUG_POSTFIX)
		if(NOT _target_name)
			set(_target_name ${_target})
		endif()
		if(NOT _target_postfix)
			set(_target_postfix "")
		endif()
		if(_target_type STREQUAL STATIC_LIBRARY)
			set_target_properties(${_target} PROPERTIES COMPILE_PDB_NAME "${_target_name}" COMPILE_PDB_NAME_DEBUG "${_target_name}${_target_postfix}")
			install(FILES "$<TARGET_FILE_DIR:${_target}>/$<$<NOT:$<CONFIG:Debug>>:$<TARGET_PROPERTY:${_target},COMPILE_PDB_NAME>>$<$<CONFIG:Debug>:$<TARGET_PROPERTY:${_target},COMPILE_PDB_NAME_DEBUG>>.pdb" DESTINATION ${CMAKE_INSTALL_LIBDIR} OPTIONAL)
		elseif(NOT _target_type STREQUAL INTERFACE_LIBRARY)
			set_target_properties(${_target} PROPERTIES PDB_NAME "${_target_name}" PDB_NAME_DEBUG "${_target_name}${_target_postfix}")
			install(FILES "$<TARGET_PDB_FILE:${_target}>" DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL)
		endif()
	endif()
endfunction()

function(primal_set_defaults)
	if(NOT DEFINED PROJECT_NAME)
		message(SEND_ERROR "PROJECT_NAME has not been defined")
		return()
	endif()
	if(PROJECT_IS_TOP_LEVEL)
		set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "qt")
		set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ".cmake")
		set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	elseif(NOT CMAKE_FOLDER)
		set(CMAKE_FOLDER ".subprojects" PARENT_SCOPE)
	endif()
	set(CMAKE_CXX_STANDARD 23 PARENT_SCOPE)
	set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG "Debug;;Release" PARENT_SCOPE) # "Release" may be the only configuration provided by the host system.
	set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL "MinSizeRel;Release;RelWithDebInfo;" PARENT_SCOPE)
	set(CMAKE_MAP_IMPORTED_CONFIG_RELEASE "Release;RelWithDebInfo;MinSizeRel;" PARENT_SCOPE)
	set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO "RelWithDebInfo;Release;MinSizeRel;" PARENT_SCOPE)
endfunction()

function(primal_set_folder _folder)
	if(NOT DEFINED PROJECT_NAME)
		message(SEND_ERROR "PROJECT_NAME has not been defined")
		return()
	endif()
	if(PROJECT_IS_TOP_LEVEL)
		set(CMAKE_FOLDER ${_folder} PARENT_SCOPE)
	endif()
endfunction()
