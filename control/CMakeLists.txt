cmake_minimum_required(VERSION 3.8)
project("MainControl" LANGUAGES CXX)

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

file(GLOB SOURCES_COMMUNICATION "../communication/src/*.cpp")
file(GLOB HEADERS_COMMUNICATION "../communication/include/*.h")

file(GLOB SOCKETS_COMMUNICATIONS "../communication/sockets/*.*" )

file(GLOB PARSER "../parser_json/src/*.*")

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES} ${PARSER} ${SOURCES_COMMUNICATION} ${HEADERS_COMMUNICATION} ${SOCKETS_COMMUNICATIONS} ../logger/logger.h ../logger/logger.cpp src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${BSON_LIBRARIES})

# Test executable, including additional source files
file(GLOB TEST_SOURCES "test/*.cpp")
add_executable(RunTests ${SOURCES} ${PARSER} ${SOURCES_COMMUNICATION} ${HEADERS_COMMUNICATION} ${SOCKETS_COMMUNICATIONS} ../logger/logger.h ../logger/logger.cpp ${TEST_SOURCES})
target_include_directories(RunTests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(RunTests PRIVATE ${BSON_LIBRARIES} gtest_main)