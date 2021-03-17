# This file is part of the Primal library.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19)
project(PrimalImport LANGUAGES CXX)
find_package(Primal REQUIRED)
primal_set_defaults()
primal_provide_fmt(fmt_ROOT)
find_package(fmt REQUIRED)
add_executable(primal_import main.cpp)
target_link_libraries(primal_import PRIVATE fmt::fmt Primal::primal)
