# ==============================================================================
# Copyright (C) 2019 floor10
#
# SPDX-License-Identifier: MIT
# ==============================================================================

project(upscaler)

set(TARGET_NAME "gstinterpolator")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER gstreamer-1.0 REQUIRED)

set(PROJECT_SOURCES
    gstinterpolator.c
)

set(PROJECT_HEADERS
    gstinterpolator.h
)

add_library(${TARGET_NAME} SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})


target_include_directories(${TARGET_NAME}
PRIVATE
        ${GSTREAMER_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${TARGET_NAME}
PRIVATE
        ${GSTREAMER_LIBRARIES}
)
