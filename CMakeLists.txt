cmake_minimum_required(VERSION 3.5)

# Set the project name and version
project(DraggableSquares VERSION 1.0)

# Find the Qt5 library
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui)

# Include current source directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Add the main application files

add_executable(DraggableSquares src/main.cpp src/draggable_square.cpp src/process.cpp src/process_dialog.cpp src/main_window.cpp)

# Use the Qt5 Widgets module
target_link_libraries(DraggableSquares Qt5::Core Qt5::Gui Qt5::Widgets)


