cmake_minimum_required(VERSION 3.8)
project("CMakeProject" LANGUAGES CXX)

# Path to local GoogleTest directory
set(GOOGLETEST_PATH "${CMAKE_SOURCE_DIR}/googletest")

# Add GoogleTest
add_subdirectory(${GOOGLETEST_PATH} ${CMAKE_BINARY_DIR}/googletest)

# Find and include bson library
find_package(PkgConfig REQUIRED)
pkg_check_modules(BSON REQUIRED libbson-1.0)
include_directories(${BSON_INCLUDE_DIRS})

# Collect source files
file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

# Main executable
add_executable(CMakeProject ${SOURCES} src/main.cpp)
target_include_directories(CMakeProject PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(CMakeProject PRIVATE ${BSON_LIBRARIES})

# Test executable, including additional source files
file(GLOB TEST_SOURCES "test/*.cpp")
add_executable(RunTests ${SOURCES} ${TEST_SOURCES})
target_include_directories(RunTests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(RunTests PRIVATE ${BSON_LIBRARIES} gtest_main)

# Copy output.bson to the build directory
configure_file(${CMAKE_SOURCE_DIR}/my_bson.bson ${CMAKE_BINARY_DIR}/my_bson.bson COPYONLY)