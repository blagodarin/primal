# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

include(GNUInstallDirs)

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
