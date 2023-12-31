# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_lupdate LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

if(CMAKE_VERSION VERSION_LESS "3.19" AND MSVC AND QT_FEATURE_debug_and_release)
    message(WARNING "tst_lupdate will not be built in this configuration.")
    return()
endif()

#####################################################################
## tst_lupdate Test:
#####################################################################

qt_internal_add_test(tst_lupdate
    SOURCES
        tst_lupdate.cpp
    LIBRARIES
        Qt::Tools
        Qt::CorePrivate
)

# The recursive tests take a bit longer for the clang-based parser.
# Increase the test timeout.
set_property(TEST tst_lupdate APPEND PROPERTY ENVIRONMENT "QTEST_FUNCTION_TIMEOUT=900000")
