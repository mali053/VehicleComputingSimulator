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

# Find Qt5 packages
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Collect source and header files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Add the executable target
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

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

# Copy JSON files to the build directory
configure_file(${CMAKE_SOURCE_DIR}/speed_sensor.json ${CMAKE_BINARY_DIR}/speed_sensor.json COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/tire_pressure_sensor.json ${CMAKE_BINARY_DIR}/tire_pressure_sensor.json COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/communication_sensor.json ${CMAKE_BINARY_DIR}/communication_sensor.json COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/camera_sensor.json ${CMAKE_BINARY_DIR}/camera_sensor.json COPYONLY)

# Copy BSON file to the build directory
configure_file(${CMAKE_SOURCE_DIR}/my_bson.bson ${CMAKE_BINARY_DIR}/my_bson.bson COPYONLY)
