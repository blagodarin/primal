# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19)
project(PrimalImport LANGUAGES CXX)
find_package(Primal REQUIRED)
primal_set_defaults()
if(WIN32)
	primal_provide_jpeg(JPEG_ROOT)
endif()
find_package(JPEG REQUIRED)
add_executable(primal_import main.cpp)
target_link_libraries(primal_import PRIVATE JPEG::JPEG Primal::primal)
