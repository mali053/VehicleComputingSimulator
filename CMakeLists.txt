cmake_minimum_required(VERSION 3.10)
project(MyTest)

set(CMAKE_CXX_STANDARD 11)

# Include the directory where AES.h is located
include_directories(src)

# Add source files
file(GLOB SOURCES "src/*.cpp")

add_executable(runTests tests/aes_tests.cpp ${SOURCES})
target_link_libraries(runTests gtest gtest_main pthread)
