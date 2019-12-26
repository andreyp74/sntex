cmake_minimum_required (VERSION 3.2)

project(Consumer CXX C)

include_directories(include src "${POCO_INCLUDE_DIRS}")

file(GLOB_RECURSE CONSUMER_SRCS
         RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
         ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_executable(${PROJECT_NAME} ${CONSUMER_SRCS})
add_dependencies(${PROJECT_NAME} poco)


if(UNIX)
    target_link_libraries(${PROJECT_NAME} ${POCO_LIBRARIES})
endif(UNIX)

target_link_libraries(
    ${PROJECT_NAME} 
    ${CMAKE_THREAD_LIBS_INIT}
)

set_target_properties(${PROJECT_NAME} 
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# ENABLE WITH:  -DBUILD_TESTS=ON
option (BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    include(consumer-tests.cmake)
endif()