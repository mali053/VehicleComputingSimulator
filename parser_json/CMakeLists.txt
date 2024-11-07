cmake_minimum_required(VERSION 3.10)

# Set the project name
project(PacketParser)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the source files
set(SRC_FILES
    src/packet_parser.cpp
)

# Include directories
include_directories(src)

# Add the main library (optional if you want to separate test and main project)
add_library(PacketParserLib ${SRC_FILES})

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add the test executable
add_executable(runTests test/test_packet_parser.cpp ${SRC_FILES})

# Link the test executable against GTest and pthread
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)