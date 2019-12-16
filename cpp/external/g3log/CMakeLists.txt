cmake_minimum_required(VERSION 3.2)
project(g3logger_builder C CXX)

include(ExternalProject)

ExternalProject_Add(g3log
    URL https://github.com/KjellKod/g3log/archive/1.3.zip 
    URL_MD5 8235e351007503765f10043743ef56a1
    CMAKE_ARGS  -DADD_BUILD_WIN_SHARED=ON
                -DADD_FATAL_EXAMPLE=OFF
                -DADD_G3LOG_UNIT_TEST=OFF
                -DUSE_DYNAMIC_LOGGING_LEVELS=ON
                -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG:PATH=${CMAKE_BINARY_DIR}/Debug
                -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE:PATH=${CMAKE_BINARY_DIR}/Release
                -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
                -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
                  
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    INSTALL_COMMAND ""
    DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/.downloaded"
)

# Specify include dir
ExternalProject_Get_Property(g3log source_dir)
set(G3LOG_INCLUDE_DIRS ${source_dir}/src PARENT_SCOPE)

# Specify g3log's link libraries
ExternalProject_Get_Property(g3log binary_dir)
set(G3LOG_LIBRARY_DIRS ${binary_dir} PARENT_SCOPE)

set(G3LOG_LIBRARIES ${CMAKE_SHARED_LIBRARY_PREFIX}g3logger${CMAKE_SHARED_LIBRARY_SUFFIX} PARENT_SCOPE)
