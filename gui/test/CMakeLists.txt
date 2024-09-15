cmake_minimum_required(VERSION 3.5)

# Set the project name and version
project(DraggableSquaresTests VERSION 1.0)

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

# Test for MainWindow
add_executable(MainWindowTests test_main_window.cpp)
target_link_libraries(MainWindowTests PRIVATE DraggableSquaresLib Qt5::Test Qt5::Gui Qt5::Widgets)
add_test(NAME MainWindowTests COMMAND MainWindowTests)

# Test for DraggableSquare
add_executable(DraggableSquareTests test_draggable_square.cpp)
target_link_libraries(DraggableSquareTests PRIVATE DraggableSquaresLib Qt5::Test Qt5::Gui Qt5::Widgets)
add_test(NAME DraggableSquareTests COMMAND DraggableSquareTests)

# Test for ProcessDialog
add_executable(ProcessDialogTests test_process_dialog.cpp)
target_link_libraries(ProcessDialogTests PRIVATE DraggableSquaresLib Qt5::Test Qt5::Gui Qt5::Widgets)
add_test(NAME ProcessDialogTests COMMAND ProcessDialogTests)

# Test for Process
add_executable(ProcessTests test_process.cpp)
target_link_libraries(ProcessTests PRIVATE DraggableSquaresLib Qt5::Test)
add_test(NAME ProcessTests COMMAND ProcessTests)

# Test for UserInteraction
add_executable(UserInteractionTests user_interaction_tests.cpp)
target_link_libraries(UserInteractionTests PRIVATE DraggableSquaresLib Qt5::Test Qt5::Gui Qt5::Widgets)
add_test(NAME UserInteractionTests COMMAND UserInteractionTests)

# Test for LogHandler
add_executable(LogHandlerTests test_log_handler.cpp)
target_link_libraries(LogHandlerTests PRIVATE DraggableSquaresLib Qt5::Test Qt5::Gui Qt5::Widgets)
add_test(NAME LogHandlerTests COMMAND LogHandlerTests)