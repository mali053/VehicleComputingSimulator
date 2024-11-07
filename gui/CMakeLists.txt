cmake_minimum_required(VERSION 3.5)

# Set the project name and version
project(VehicleComputingSimulator VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic generation of MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include current source directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set the -fPIC flag to enable Position Independent Code for all targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set the output directory for executables to the build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Find the Qt5 library
find_package(Qt5 REQUIRED COMPONENTS Concurrent Widgets Core Gui Test)

# Find the BSON library using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(BSON REQUIRED libbson-1.0)

# Create a library for the common code
add_library(VehicleComputingSimulatorLib
    src/draggable_square.cpp
    src/process.cpp
    src/process_dialog.cpp
    src/main_window.cpp
    src/simulation_state_manager.cpp
    src/log_handler.cpp
    src/frames.cpp
    src/compiler.cpp
    ../logger/logger.cpp
)

# Add headers to the library target
target_sources(VehicleComputingSimulatorLib PRIVATE
    include/main_window.h
    include/draggable_square.h
    include/process_dialog.h
    include/simulation_state_manager.h
    include/log_handler.h
    include/frames.h
    include/compiler.h
    ../logger/logger.h
)

# Include BSON directories and project include directory
target_include_directories(VehicleComputingSimulatorLib PRIVATE
    ${BSON_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../logger
)

# Link the BSON library and Qt5 components to the library
target_link_libraries(VehicleComputingSimulatorLib PRIVATE ${BSON_LIBRARIES} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent)

# Create the main executable
add_executable(VehicleComputingSimulator src/main.cpp)

# Link the main executable with the library and Qt5 modules
target_link_libraries(VehicleComputingSimulator PRIVATE VehicleComputingSimulatorLib Qt5::Core Qt5::Gui Qt5::Widgets)

# Make sure the main executable has access to the include directory
target_include_directories(VehicleComputingSimulator PRIVATE ${CMAKE_SOURCE_DIR}/include ${BSON_INCLUDE_DIRS})

# Enable testing
enable_testing()

# Add the test directory
add_subdirectory(test)
