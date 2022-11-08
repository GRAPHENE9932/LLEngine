cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(LLShooter)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

enable_testing()
add_subdirectory(src)
add_subdirectory(tst)

# Copy resources to the binary directory.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/res" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/res")
