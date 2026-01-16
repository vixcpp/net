#  @file CMakeLists.cpp
#  @author Gaspard Kirira
#
#  Copyright 2025, Gaspard Kirira.  All rights reserved.
#  https://github.com/vixcpp/vix
#  Use of this source code is governed by a MIT license
#  that can be found in the License file.
#
#  Vix.cpp
# NET MODULE
# Purpose:
#   Low-level networking primitives for Vix (connectivity probing,
#   reachability checks, lightweight network utilities). Builds as
#   STATIC when sources exist, otherwise as a header-only INTERFACE target.
#
# Public Targets:
#   - vix_net   : The actual library target (STATIC or INTERFACE)
#   - vix::net  : Namespaced alias for consumers
#
# Public Dependencies:
#   - vix::utils
#
# Options:
#   - VIX_ENABLE_SANITIZERS : Inherit sanitizers from the parent project
#
# Installation/Export:
#   Installs into the umbrella export-set `VixTargets`.
# ====================================================================


cmake_minimum_required(VERSION 3.20)
project(vix_net VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

# Global settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Sources discovery
file(GLOB_RECURSE NET_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Utils dependency
option(VIX_NET_FETCH_UTILS "Auto-fetch vix::utils if missing" ON)

if (NOT TARGET vix::utils)
  if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../utils/CMakeLists.txt")
    message(STATUS "[net] Adding utils from umbrella: ../utils")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../utils" "utils")
  elseif (VIX_NET_FETCH_UTILS)
    include(FetchContent)
    message(STATUS "[net] Fetching vix::utils via FetchContent")
    FetchContent_Declare(vix_utils
      GIT_REPOSITORY https://github.com/vixcpp/utils.git
      GIT_TAG       dev
    )
    FetchContent_MakeAvailable(vix_utils)
  else()
    message(FATAL_ERROR "vix::utils not found. Enable VIX_NET_FETCH_UTILS=ON or provide the target before net.")
  endif()
endif()

set(VIX_UTILS_TARGET vix::utils)
if (NOT TARGET ${VIX_UTILS_TARGET} AND TARGET vix::utils)
  set(VIX_UTILS_TARGET vix::utils)
endif()

# STATIC
if (NET_SOURCES)
  message(STATUS "[net] Building STATIC library with detected sources.")

  add_library(vix_net STATIC ${NET_SOURCES})

  if (TARGET vix_warnings)
    message(STATUS "[net] vix_warnings detected (from umbrella)")
    target_link_libraries(vix_net PUBLIC vix_warnings)
    if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
      target_link_libraries(vix_net INTERFACE vix_sanitizers)
    endif()
  else()
    message(STATUS "[net] vix_warnings not found (building net standalone)")
  endif()

  add_library(vix::net ALIAS vix_net)
  target_compile_features(vix_net PUBLIC cxx_std_20)

  target_include_directories(vix_net
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  target_link_libraries(vix_net
    PUBLIC
      ${VIX_UTILS_TARGET}
  )

  set_target_properties(vix_net PROPERTIES
    OUTPUT_NAME vix_net
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    EXPORT_NAME net
  )

  install(TARGETS vix_net
    EXPORT VixTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
          FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

# HEADER-ONLY
else()
  message(STATUS "[net] Building HEADER-ONLY library (no sources).")

  add_library(vix_net INTERFACE)
  add_library(vix::net ALIAS vix_net)
  target_compile_features(vix_net INTERFACE cxx_std_20)

  target_include_directories(vix_net INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  target_link_libraries(vix_net INTERFACE
    ${VIX_UTILS_TARGET}
  )

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    target_link_libraries(vix_net INTERFACE vix_sanitizers)
  endif()

  install(TARGETS vix_net
    EXPORT VixTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
          FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
endif()

# Tests
option(VIX_NET_BUILD_TESTS "Build net module tests" OFF)

if (VIX_NET_BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()

# Summary
message(STATUS "------------------------------------------------------")
message(STATUS "vix::net configured (${PROJECT_VERSION})")
if (NET_SOURCES)
  message(STATUS "Mode: STATIC / sources found")
else()
  message(STATUS "Mode: HEADER-ONLY / no sources")
endif()
message(STATUS "Utils target:    ${VIX_UTILS_TARGET}")
message(STATUS "Include dir: ${CMAKE_CURRENT_SOURCE_DIR}/include (for <vix/...>)")
message(STATUS "Sanitizers (inherited): ${VIX_ENABLE_SANITIZERS}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")
