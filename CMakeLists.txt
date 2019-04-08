# ==============================================================================
# Copyright (C) 2019 floor10
#
# SPDX-License-Identifier: MIT
# ==============================================================================

cmake_minimum_required(VERSION 3.6)

project(UpScalerPlugins)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(PLUGIN_VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

add_definitions(-DPLUGINS_VERSION="${PLUGIN_VERSION}")
add_definitions(-DPLUGINS_LICENSE="MIT")
add_definitions(-DPACKAGE="gst-super-scaling")
add_definitions(-DPACKAGE_NAME="GStreamer super scaling elements")
add_definitions(-DGST_PACKAGE_ORIGIN="https://github.com/floor10/upscaler")

configure_file(cmake/config.h.in configs/config.h @ONLY)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/configs)

add_subdirectory(gst)
