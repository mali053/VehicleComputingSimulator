cmake_minimum_required(VERSION 3.5)

# Set the project name and version
project(DraggableSquares VERSION 1.0)
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

# Find the BSON library
find_package(PkgConfig REQUIRED)
pkg_check_modules(BSON REQUIRED libbson-1.0)

# Add the main application files
add_executable(DraggableSquares
    src/main.cpp
    src/draggable_square.cpp
    src/process.cpp
    src/process_dialog.cpp
    src/main_window.cpp
    src/simulation_data_manager.cpp
    src/log_handler.cpp
    src/frames.cpp
)

# Include BSON directories
target_include_directories(DraggableSquares PRIVATE ${BSON_INCLUDE_DIRS})

# Link the BSON library and Qt5 components
target_link_libraries(DraggableSquares PRIVATE ${BSON_LIBRARIES} Qt5::Core Qt5::Gui Qt5::Widgets)

# Enable testing
enable_testing()

# Add test directory
add_subdirectory(test)