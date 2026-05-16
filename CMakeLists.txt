# ====================================================================
#  @file CMakeLists.txt
#  @author Gaspard Kirira
#
#  Copyright 2026, Gaspard Kirira. All rights reserved.
#  https://github.com/vixcpp/vix
#  Use of this source code is governed by a MIT license
#  that can be found in the License file.
#
# ====================================================================
# Vix.cpp - Net Module
# ====================================================================
# Purpose:
#   Low-level networking primitives for Vix.
#   Provides connectivity probing and outbound HTTP client abstractions.
#
# Public Targets:
#   - vix_net   : The actual library target
#   - vix::net  : Namespaced alias for consumers
#
# Public Dependencies:
#   - vix::error
#   - vix::process
#
# Installation/Export:
#   - Umbrella build exports into: VixTargets
#   - Standalone build exports into: vix_netTargets
# ====================================================================

cmake_minimum_required(VERSION 3.20)
project(vix_net VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# --------------------------------------------------------------------
# Global settings
# --------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --------------------------------------------------------------------
# Export set selection
# --------------------------------------------------------------------
if (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
  set(VIX_NET_EXPORT_SET VixTargets)
else()
  set(VIX_NET_EXPORT_SET vix_netTargets)
endif()

# --------------------------------------------------------------------
# Umbrella build policy
# --------------------------------------------------------------------
if (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
  set(VIX_NET_FETCH_ERROR   OFF CACHE BOOL "Auto-fetch vix::error if missing" FORCE)
  set(VIX_NET_FETCH_PROCESS OFF CACHE BOOL "Auto-fetch vix::process if missing" FORCE)
  set(VIX_NET_FETCH_TESTS   OFF CACHE BOOL "Auto-fetch vix::tests if missing" FORCE)
endif()

# --------------------------------------------------------------------
# Sources discovery
# --------------------------------------------------------------------
file(GLOB_RECURSE NET_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# --------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------
option(VIX_NET_FETCH_ERROR   "Auto-fetch vix::error if missing" ON)
option(VIX_NET_FETCH_PROCESS "Auto-fetch vix::process if missing" ON)
option(VIX_NET_FETCH_TESTS   "Auto-fetch vix::tests if missing" ON)

# error
if (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
  if (NOT TARGET vix::error)
    message(FATAL_ERROR "[net] Umbrella build: vix::error must be provided by umbrella before net.")
  endif()
else()
  if (NOT TARGET vix::error)
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../error/CMakeLists.txt")
      message(STATUS "[net] Adding error from sibling: ../error")
      add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../error" "error")
    else()
      find_package(vix_error CONFIG QUIET)
    endif()

    if (NOT TARGET vix::error)
      if (VIX_NET_FETCH_ERROR)
        include(FetchContent)
        message(STATUS "[net] Fetching vix::error via FetchContent")
        FetchContent_Declare(vix_error
          GIT_REPOSITORY https://github.com/vixcpp/error.git
          GIT_TAG        v0.1.0
        )
        FetchContent_MakeAvailable(vix_error)
      else()
        message(FATAL_ERROR
          "vix::error not found. Enable VIX_NET_FETCH_ERROR=ON, install vix_error, or provide the target before net."
        )
      endif()
    endif()
  endif()
endif()

# process
if (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
  if (NOT TARGET vix::process)
    message(FATAL_ERROR "[net] Umbrella build: vix::process must be provided by umbrella before net.")
  endif()
else()
  if (NOT TARGET vix::process)
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../process/CMakeLists.txt")
      message(STATUS "[net] Adding process from sibling: ../process")
      add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../process" "process")
    else()
      find_package(vix_process CONFIG QUIET)
    endif()

    if (NOT TARGET vix::process)
      if (VIX_NET_FETCH_PROCESS)
        include(FetchContent)
        message(STATUS "[net] Fetching vix::process via FetchContent")
        FetchContent_Declare(vix_process
          GIT_REPOSITORY https://github.com/vixcpp/process.git
          GIT_TAG        v0.1.0
        )
        FetchContent_MakeAvailable(vix_process)
      else()
        message(FATAL_ERROR
          "vix::process not found. Enable VIX_NET_FETCH_PROCESS=ON, install vix_process, or provide the target before net."
        )
      endif()
    endif()
  endif()
endif()

set(VIX_ERROR_TARGET   vix::error)
set(VIX_PROCESS_TARGET vix::process)

# --------------------------------------------------------------------
# Library target
# --------------------------------------------------------------------
if (NET_SOURCES)
  message(STATUS "[net] Building STATIC library with detected sources.")

  add_library(vix_net STATIC ${NET_SOURCES})
  add_library(vix::net ALIAS vix_net)

  target_compile_features(vix_net PUBLIC cxx_std_20)

  target_include_directories(vix_net
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  target_link_libraries(vix_net
    PUBLIC
      ${VIX_ERROR_TARGET}
      ${VIX_PROCESS_TARGET}
  )

  if (NOT MSVC)
    target_compile_options(vix_net PRIVATE
      -Wall
      -Wextra
      -Wpedantic
    )
  endif()

  if (VIX_ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(vix_net PRIVATE
      -fno-omit-frame-pointer
      -fsanitize=address,undefined
    )
    target_link_options(vix_net PRIVATE
      -fsanitize=address,undefined
    )
  endif()

  set_target_properties(vix_net PROPERTIES
    OUTPUT_NAME vix_net
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    EXPORT_NAME net
  )

  install(TARGETS vix_net
    EXPORT ${VIX_NET_EXPORT_SET}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

else()
  message(STATUS "[net] Building HEADER-ONLY library.")

  add_library(vix_net INTERFACE)
  add_library(vix::net ALIAS vix_net)

  target_compile_features(vix_net INTERFACE cxx_std_20)

  target_include_directories(vix_net INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  target_link_libraries(vix_net
    INTERFACE
      ${VIX_ERROR_TARGET}
      ${VIX_PROCESS_TARGET}
  )

  set_target_properties(vix_net PROPERTIES
    EXPORT_NAME net
  )

  install(TARGETS vix_net
    EXPORT ${VIX_NET_EXPORT_SET}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
endif()

# --------------------------------------------------------------------
# Install headers
# --------------------------------------------------------------------
if (NOT (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD))
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
      PATTERN "*.hpp"
      PATTERN "*.h"
  )
endif()

# --------------------------------------------------------------------
# Standalone package config/export
# --------------------------------------------------------------------
if (NOT (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD))

  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/vix_netConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/vix_netConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vix_net"
  )

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/vix_netConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
  )

  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/vix_netConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/vix_netConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vix_net"
  )

  install(EXPORT vix_netTargets
    FILE vix_netTargets.cmake
    NAMESPACE vix::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vix_net"
  )

endif()

# --------------------------------------------------------------------
# Tests dependency
# --------------------------------------------------------------------
option(VIX_NET_BUILD_TESTS "Build net module tests" OFF)

if (VIX_NET_BUILD_TESTS)
  if (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
    if (NOT TARGET vix::tests)
      message(FATAL_ERROR "[net] Umbrella build: vix::tests must be provided before net tests.")
    endif()
  else()
    if (NOT TARGET vix::tests)
      if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../tests/CMakeLists.txt")
        message(STATUS "[net] Adding tests from sibling: ../tests")
        add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../tests" "tests")
      else()
        find_package(vix_tests CONFIG QUIET)
      endif()

      if (NOT TARGET vix::tests)
        if (VIX_NET_FETCH_TESTS)
          include(FetchContent)
          message(STATUS "[net] Fetching vix::tests via FetchContent")
          FetchContent_Declare(vix_tests
            GIT_REPOSITORY https://github.com/vixcpp/tests.git
            GIT_TAG        v0.1.0
          )
          FetchContent_MakeAvailable(vix_tests)
        else()
          message(FATAL_ERROR "vix::tests not found.")
        endif()
      endif()
    endif()
  endif()

  include(CTest)
  enable_testing()

  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
  elseif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
    add_subdirectory(test)
  endif()
endif()

# --------------------------------------------------------------------
# Summary
# --------------------------------------------------------------------
message(STATUS "------------------------------------------------------")
message(STATUS "vix::net configured (${PROJECT_VERSION})")
if (NET_SOURCES)
  message(STATUS "Mode: STATIC / sources found")
else()
  message(STATUS "Mode: HEADER-ONLY / no sources")
endif()
message(STATUS "Export set:     ${VIX_NET_EXPORT_SET}")
message(STATUS "Error target:   ${VIX_ERROR_TARGET}")
message(STATUS "Process target: ${VIX_PROCESS_TARGET}")
message(STATUS "Include dir:    ${CMAKE_CURRENT_SOURCE_DIR}/include (for <vix/net/...>)")
message(STATUS "Build type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")
