cmake_minimum_required(VERSION 3.16)

# Project name and settings
project(BuildCondition VERSION 1.0 LANGUAGES CXX)

# Set C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt specific features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# Option to enable building tests
option(BUILD_TESTS "Build tests" ON)

# Find Qt5 packages
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Collect source and header files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

# Add the executable target
add_executable(${PROJECT_NAME} ${SOURCES}  src/main.cpp ${HEADERS} ../logger/logger.h ../logger/logger.cpp)

# Include directories for the project
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link Qt to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)

# Find libbson using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBBSON REQUIRED libbson-1.0)

# Include libbson headers and link libbson library
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBBSON_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBBSON_LIBRARIES})

# Add GoogleTest
add_subdirectory(${CMAKE_SOURCE_DIR}/googletest)

enable_testing()

# Collect test source files
file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")

# Add the test executable
add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES} ${SOURCES} ${HEADERS} ../logger/logger.h ../logger/logger.cpp)

# Link GoogleTest libraries
target_link_libraries(${PROJECT_NAME}_tests PRIVATE gtest gtest_main Qt5::Widgets ${LIBBSON_LIBRARIES})

# Include directories for tests
target_include_directories(${PROJECT_NAME}_tests PRIVATE ${CMAKE_SOURCE_DIR}/include ${LIBBSON_INCLUDE_DIRS})

# Add test cases
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)

# Copy the  icon folder into the build folder
file(COPY ${CMAKE_SOURCE_DIR}/src/icons DESTINATION ${CMAKE_BINARY_DIR})