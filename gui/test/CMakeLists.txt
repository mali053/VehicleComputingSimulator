cmake_minimum_required(VERSION 3.5)

# Set the project name and version
project(VehicleComputingSimulatorTests VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic generation of MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include current source directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the Qt5 library
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui Test)

# Link BSON library
find_package(PkgConfig REQUIRED)
pkg_check_modules(BSON REQUIRED libbson-1.0)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include ${BSON_INCLUDE_DIRS})

# Set the runtime output directory for all test executables to the build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Helper to support both direct and ctest execution
function(add_test_with_working_dir target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} PRIVATE VehicleComputingSimulatorLib Qt5::Test Qt5::Gui Qt5::Widgets)
    set_target_properties(${target_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    add_test(NAME ${target_name} COMMAND ${target_name})
    set_tests_properties(${target_name} PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endfunction()

# Tests
add_test_with_working_dir(MainWindowTests test_main_window.cpp)
add_test_with_working_dir(DraggableSquareTests test_draggable_square.cpp)
add_test_with_working_dir(ProcessDialogTests test_process_dialog.cpp)
add_test_with_working_dir(ProcessTests test_process.cpp)
add_test_with_working_dir(UserInteractionTests user_interaction_tests.cpp)
add_test_with_working_dir(LogHandlerTests test_log_handler.cpp)
