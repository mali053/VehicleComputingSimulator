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

file(GLOB SOURCES_COMMUNICATION "../communication/src/*.cpp")
file(GLOB HEADERS_COMMUNICATION "../communication/src/*.h")

file(GLOB PARSER "../parser_json/src/*.*")

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES} ${SOURCES_COMMUNICATION} ${HEADERS_COMMUNICATION} ${PARSER} ../logger/logger.h ../logger/logger.cpp src/main.cpp)
target_include_directories(CMakeProject PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(CMakeProject PRIVATE ${BSON_LIBRARIES})

# Test executable, including additional source files
file(GLOB TEST_SOURCES "test/*.cpp")
add_executable(RunTests ${SOURCES} ${SOURCES_COMMUNICATION} ${HEADERS_COMMUNICATION} ${PARSER} ../logger/logger.h ../logger/logger.cpp ${TEST_SOURCES})
target_include_directories(RunTests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(RunTests PRIVATE ${BSON_LIBRARIES} gtest_main)

# Copy .json to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/src/sensors_data DESTINATION ${CMAKE_BINARY_DIR})

# Copy output.bson and config.json to the build directory
configure_file(${CMAKE_SOURCE_DIR}/../conditions.bson ${CMAKE_BINARY_DIR}/conditions.bson COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)