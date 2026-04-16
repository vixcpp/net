# ====================================================================
#  @file CMakeLists.txt
#  @author Gaspard Kirira
#
#  Copyright 2025, Gaspard Kirira.  All rights reserved.
#  https://github.com/vixcpp/vix
#  Use of this source code is governed by a MIT license
#  that can be found in the License file.
#
# ====================================================================
# Vix.cpp - Net Module
# ====================================================================
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
# Export set selection  ✅ CRUCIAL
# --------------------------------------------------------------------
if (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
  set(VIX_NET_EXPORT_SET VixTargets)
else()
  set(VIX_NET_EXPORT_SET vix_netTargets)
endif()

# --------------------------------------------------------------------
# Sources discovery
# --------------------------------------------------------------------
file(GLOB_RECURSE NET_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

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

  # Build-only helpers (ne doivent pas être exportés)
  if (TARGET vix_warnings)
    target_link_libraries(vix_net PRIVATE vix_warnings)
  endif()

  if (VIX_ENABLE_SANITIZERS AND TARGET vix_sanitizers)
    target_link_libraries(vix_net PRIVATE vix_sanitizers)
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

  set_target_properties(vix_net PROPERTIES
    EXPORT_NAME net
  )

  install(TARGETS vix_net
    EXPORT ${VIX_NET_EXPORT_SET}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
endif()

# --------------------------------------------------------------------
# Install headers (standalone only)
# --------------------------------------------------------------------
if (NOT (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD))
  install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
  )
endif()

# --------------------------------------------------------------------
# Standalone package config/export
# IMPORTANT: uniquement hors umbrella
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
# Tests
# --------------------------------------------------------------------
option(VIX_NET_BUILD_TESTS "Build net module tests" OFF)

if (VIX_NET_BUILD_TESTS)
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
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
message(STATUS "Export set:       ${VIX_NET_EXPORT_SET}")
message(STATUS "Include dir:      ${CMAKE_CURRENT_SOURCE_DIR}/include (for <vix/...>)")
message(STATUS "Sanitizers:       ${VIX_ENABLE_SANITIZERS}")
message(STATUS "Build type:       ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")
