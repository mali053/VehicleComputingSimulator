cmake_minimum_required(VERSION 3.8)
project("CMakeProject" LANGUAGES CXX)

# Path to local GoogleTest directory
set(GOOGLETEST_PATH "${CMAKE_SOURCE_DIR}/googletest")

# Add GoogleTest
add_subdirectory(${GOOGLETEST_PATH} ${CMAKE_BINARY_DIR}/googletest)

# Find all source files except for main.cpp
file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

# Main executable
add_executable(CMakeProject ${SOURCES} src/main.cpp)
target_include_directories(CMakeProject PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Test executable, including additional source files (without main.cpp)
file(GLOB TEST-SOURCES "test/*.cpp")
add_executable(RunTests ${SOURCES} ${TEST-SOURCES})
target_link_libraries(RunTests PRIVATE gtest_main)
target_include_directories(RunTests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Copy json_to_read.json to the build directory
configure_file(${CMAKE_SOURCE_DIR}/json_to_read.json ${CMAKE_BINARY_DIR}/json_to_read.json COPYONLY)