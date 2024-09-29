cmake_minimum_required(VERSION 3.10)
project(ImageProcessing)

set(CMAKE_CXX_STANDARD 17)

# adding libraries
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories(include ../logger ../communication/include ../communication/sockets)

# make the library with the src files
file(GLOB SOURCES "src/*.cpp" "../logger/*.cpp" "tests/utils.cpp" "../communication/src/*.cpp" "../communication/sockets/*.cpp")
add_library(ImageProcessingLib ${SOURCES})

# create CommunicationLib library
add_library(CommunicationLib STATIC ../communication/src/communication.cpp ../communication/src/client_connection.cpp ../communication/src/message.cpp ../communication/src/packet.cpp ../communication/src/bus_manager.cpp ../communication/src/server_connection.cpp ../logger/logger.cpp)

configure_file( ${CMAKE_BINARY_DIR}/config.json COPYONLY)
# create test executable
##add_executable(runTests tests/main.cpp tests/test_serialize.cpp tests/test_detector.cpp tests/test_dynamic_tracker.cpp tests/test_distance.cpp tests/test_manager.cpp tests/test_velocity.cpp)
##target_link_libraries(runTests ImageProcessingLib ${OpenCV_LIBS} ${GTEST_LIBRARIES} pthread CommunicationLib)

# adding tests
##enable_testing()
##add_test(NAME runTests COMMAND runTests)

add_executable(runMain src/main.cpp)
target_link_libraries(runMain ImageProcessingLib CommunicationLib ${OpenCV_LIBS})
