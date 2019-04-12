# ==============================================================================
# Copyright (C) 2019 floor10
#
# SPDX-License-Identifier: MIT
# ==============================================================================

cmake_minimum_required(VERSION 3.6)

set(TARGET_NAME "gstsuperesolution")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER gstreamer-1.0 REQUIRED)

file(GLOB PROJECT_SOURCES
    interpolator/*.c
    interpolator/*.cpp
    upscaler/*.c
    gstelements.c
)

file(GLOB PROJECT_HEADERS
    interpolator/*.h
    upscaler/*.h
)

add_library(${TARGET_NAME} SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})


target_include_directories(${TARGET_NAME}
PRIVATE
        ${GSTREAMER_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
        interpolator
        upscaler
)

target_link_libraries(${TARGET_NAME}
PRIVATE
        ${GSTREAMER_LIBRARIES}
)
