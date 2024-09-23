# CMakeLists.txt for /VehicleComputingSimulator/communication/main_bus
# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.10)
# Set the project name
project(VehicleComputingSimulator)
# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# Add the path to the source files
set(SOURCES
    ../communication/src/bus_manager.cpp
    ../communication/src/server_connection.cpp
    ../communication/src/packet.cpp
    ../communication/src/message.cpp
    ../../logger/logger.cpp
    ../communication/sockets/real_socket.cpp
    # Include additional source files here if needed
)
# Add the executable for main_bus
add_executable(main_bus main_bus.cpp ${SOURCES})
# Include directories for header files
include_directories(
    ../communication/src
    ../communication/sockets
    # Add more directories if needed
)
# Link the executable with the necessary libraries
target_link_libraries(main_bus
    pthread
    # Add more libraries if needed
)
