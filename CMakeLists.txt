cmake_minimum_required(VERSION 3.10)
project(basicApi)
# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
# Add source files
set(SOURCES
    src/Packet.cpp
    src/communication.cpp
    test/testCommunication.cpp
)
# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sockets)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../googletest/googletest/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../googletest/googlemock/include)
# Find and link pthread
find_package(Threads REQUIRED)
# Add GTest and GMock
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../googletest googletest-build)
# Add the test executable
add_executable(testCommunication ${SOURCES})
target_link_libraries(testCommunication gtest gmock gtest_main gmock_main Threads::Threads)
# Enable testing
enable_testing()
# Add tests
add_test(NAME testCommunication COMMAND testCommunication)