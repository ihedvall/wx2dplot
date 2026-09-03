# Copyright 2026 Ingemar Hedvall
# SPDX-License-Identifier: MIT

include(CMakePrintHelpers)
cmake_minimum_required(VERSION 3.20)

if ( ("${COMP_DIR}" STREQUAL "") AND (NOT "${CMAKE_TOOLCHAIN_FILE}" STREQUAL "") )
  set(PLOT_USE_VCPKG ON) # Needed to control the included libraries in applications
else ()
  set(PLOT_USE_VCPKG OFF)
endif()

option(BUILD_SHARED_LIBS "Build static library as default." OFF)
option(PLOT_BUILD_DOC "Build documentation. Requires Doxygen." OFF)
option(PLOT_BUILD_TOOL "Build tools like the 2DPlot Demo." OFF)
option(PLOT_BUILD_TEST "Build Google Unit Tests. Requires Google Test." OFF)

project(wx2DPlot
  VERSION 1.0
  DESCRIPTION "Simple high-level interface around the wxMathPlot library"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_DEBUG_POSTFIX d)

include (script/wxwidgets.cmake)
include (script/wxmathplot.cmake)

add_library(wx2dplot
        src/signal.cpp include/plot/signal.h
        src/signalvalue.cpp include/plot/signalvalue.h
        src/signalvaluelist.cpp include/plot/signalvaluelist.h
        src/plotlogstream.cpp include/plot/plotlogstream.h
        src/alphanumericdisplay.cpp include/plot/alphanumericdisplay.h
        src/project.cpp include/plot/project.h
        src/lineplotdisplay.cpp
        include/plot/lineplotdisplay.h)

target_include_directories(wx2dplot PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_include_directories(wx2dplot PRIVATE ${wxmathplot_SOURCE_DIR})

target_link_libraries(wx2dplot PRIVATE
        wxmathplot_static
        wxWidgets::wxWidgets)

if (PLOT_BUILD_TOOL)
  add_subdirectory(lineplot)
endif()
