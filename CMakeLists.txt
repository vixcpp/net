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
#   - None
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
include(CMakePackageConfigHelpers)

# Global settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Sources discovery
file(GLOB_RECURSE NET_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# --------------------------------------------------------------------
# Library mode
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

  if (TARGET vix_warnings)
    message(STATUS "[net] vix_warnings detected (from umbrella)")
    target_link_libraries(vix_net PUBLIC vix_warnings)
  else()
    message(STATUS "[net] vix_warnings not found (building net standalone)")
  endif()

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    target_link_libraries(vix_net PUBLIC vix_sanitizers)
  endif()

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

# --------------------------------------------------------------------
# Header-only mode
# --------------------------------------------------------------------
else()
  message(STATUS "[net] Building HEADER-ONLY library (no sources).")

  add_library(vix_net INTERFACE)
  add_library(vix::net ALIAS vix_net)

  target_compile_features(vix_net INTERFACE cxx_std_20)

  target_include_directories(vix_net INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    target_link_libraries(vix_net INTERFACE vix_sanitizers)
  endif()

  install(TARGETS vix_net
    EXPORT VixTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
endif()

# Install headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

# Package config
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

install(EXPORT VixTargets
  FILE VixTargets.cmake
  NAMESPACE vix::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vix_net"
)

# Tests
option(VIX_NET_BUILD_TESTS "Build net module tests" OFF)

if (VIX_NET_BUILD_TESTS)
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
  else()
    message(WARNING "[net/tests] VIX_NET_BUILD_TESTS=ON but tests/CMakeLists.txt not found, skipping.")
  endif()
endif()

# Summary
message(STATUS "------------------------------------------------------")
message(STATUS "vix::net configured (${PROJECT_VERSION})")
if (NET_SOURCES)
  message(STATUS "Mode: STATIC / sources found")
else()
  message(STATUS "Mode: HEADER-ONLY / no sources")
endif()
message(STATUS "Include dir: ${CMAKE_CURRENT_SOURCE_DIR}/include (for <vix/...>)")
message(STATUS "Sanitizers (inherited): ${VIX_ENABLE_SANITIZERS}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")
