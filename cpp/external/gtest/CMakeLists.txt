cmake_minimum_required(VERSION 3.2)
project(gtest_builder C CXX)

include(ExternalProject)

ExternalProject_Add(googletest
    URL https://github.com/google/googletest/archive/release-1.7.0.zip
    URL_MD5 ef5e700c8a0f3ee123e2e0209b8b4961
    CMAKE_ARGS -Dgtest_force_shared_crt=ON
               -Dgtest_build_tests=OFF
               -DBUILD_SHARED_LIBS=ON
               -DLIBRARY_OUTPUT_PATH:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
               -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
               -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}

    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    INSTALL_COMMAND ""
    DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/.downloaded"
)

# Specify include dir
ExternalProject_Get_Property(googletest SOURCE_DIR)
set(GTEST_INCLUDE_DIRS ${SOURCE_DIR}/include PARENT_SCOPE)

set(GTEST_LIBRARIES
  ${CMAKE_SHARED_LIBRARY_PREFIX}gtest${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${CMAKE_SHARED_LIBRARY_PREFIX}gtest_main${CMAKE_SHARED_LIBRARY_SUFFIX}
  PARENT_SCOPE
)
