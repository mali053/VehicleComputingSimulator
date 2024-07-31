cmake_minimum_required(VERSION 3.10)

# Project name
project(BasicApi)

# Setting standards

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Adding libraries and includes
include_directories(${CMAKE_SOURCE_DIR})

# Adding source files
set(COMMON_SOURCES
    communication.cpp
    myClass.cpp
    Packet.cpp
)

# Creating an executable file for user1
add_executable(user1 user1.cpp ${COMMON_SOURCES})

# Creating an executable file for user2
add_executable(user2 user2.cpp ${COMMON_SOURCES})

# Linking with the pthread library
target_link_libraries(user1 pthread)
target_link_libraries(user2 pthread)
