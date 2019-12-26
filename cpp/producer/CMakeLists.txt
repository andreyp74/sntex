cmake_minimum_required (VERSION 3.2)

project(Producer CXX C)

include_directories(include src "${POCO_INCLUDE_DIRS}")

file(GLOB_RECURSE PRODUCER_SRC
         RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
         ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_executable(${PROJECT_NAME} ${PRODUCER_SRC})
add_dependencies(${PROJECT_NAME} poco)

#if(MSVC)
#    target_link_libraries(${PROJECT_NAME} ws2_32 iphlpapi)
#endif(MSVC)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} ${POCO_LIBRARIES})
endif(UNIX)

target_link_libraries(
    ${PROJECT_NAME}
    #${CMAKE_THREAD_LIBS_INIT}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# ENABLE WITH:  -DBUILD_TESTS=ON
option (BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    include(producer-tests.cmake)
endif()
