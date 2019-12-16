cmake_minimum_required (VERSION 3.2)

project(UBATestClient CXX C)

include_directories(src "${OPENSSL_INCLUDE_DIR}" "${POCO_INCLUDE_DIRS}" "${CORE_INCLUDE_DIR}")

file(GLOB_RECURSE SRC
         RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
         ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

source_group(Sources FILES ${SRC})

add_executable(${PROJECT_NAME} ${SRC})
add_dependencies(${PROJECT_NAME} poco)

if(MSVC)
    target_link_libraries(${PROJECT_NAME} ws2_32 iphlpapi)
endif(MSVC)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} stdc++fs ${POCO_LIBRARIES})
endif(UNIX)

target_link_libraries(
    ${PROJECT_NAME} 
    ${CMAKE_THREAD_LIBS_INIT}
)

set_target_properties(${PROJECT_NAME} 
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})