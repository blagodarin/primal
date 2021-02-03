# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.12)
project(PrimalImport LANGUAGES CXX)
find_package(Primal REQUIRED)
primal_set_defaults()
add_executable(primal_import main.cpp)
target_link_libraries(primal_import PRIVATE Primal::primal)
