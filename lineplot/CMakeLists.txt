# Copyright 2026 Ingemar Hedvall
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(LinePlot
        VERSION 1.0
        DESCRIPTION "Simple line plotter application"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

include (../script/utillib.cmake)
include (../script/boost.cmake)

add_executable(lineplot
        src/windowid.h
        src/lineplotapp.h src/lineplotapp.cpp
        src/mainframe.cpp src/mainframe.h
        src/lineplotframe.cpp src/lineplotframe.h
        src/lineplotdocument.cpp src/lineplotdocument.h
        src/lineplotview.cpp src/lineplotview.h)

target_include_directories(lineplot PRIVATE ${wxmathplot_SOURCE_DIR})

if(MSVC)
    target_link_options(lineplot PRIVATE /SUBSYSTEM:WINDOWS)
    target_compile_definitions(lineplot PRIVATE _WIN32_WINNT=0x0A00)
endif()

if(MINGW)
    target_link_options(lineplot PRIVATE -mwindows -static -fstack-protector)
endif()

target_link_libraries(lineplot PRIVATE Boost::headers Boost::system Boost::process Boost::filesystem )
target_link_libraries(lineplot PRIVATE wx2dplot)
target_link_libraries(lineplot PRIVATE util)
target_link_libraries(lineplot PRIVATE wxmathplot_static)
target_link_libraries(lineplot PRIVATE ${wxWidgets_LIBRARIES})