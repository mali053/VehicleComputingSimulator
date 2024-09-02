cmake_minimum_required(VERSION 3.5)

# Set the project name and version
project(PacketParserTool VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
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
add_executable(PacketParserTool
    src/main.cpp
    src/packet_parser.cpp
)

# Include the src directory
target_include_directories(PacketParserTool PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Include BSON directories
target_include_directories(PacketParserTool PRIVATE ${BSON_INCLUDE_DIRS})

# Include JSON library directory (update this path as needed)
target_include_directories(PacketParserTool PRIVATE ${CMAKE_SOURCE_DIR}/third_party/json)

# Link the BSON library and Qt5 components
target_link_libraries(PacketParserTool PRIVATE ${BSON_LIBRARIES} Qt5::Core Qt5::Gui Qt5::Widgets)
