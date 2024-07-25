cmake_minimum_required(VERSION 3.10)
project(MyTest)

# Include directories
include_directories(src)

# Find GMP library 
find_path(GMP_INCLUDE_DIR NAMES gmp.h)
find_library(GMP_LIBRARY NAMES gmp)
find_library(GMPXX_LIBRARY NAMES gmpxx)

# Check if GMP libraries were found
if(NOT GMP_INCLUDE_DIR OR NOT GMP_LIBRARY OR NOT GMPXX_LIBRARY)
  message(FATAL_ERROR "Could not find GMP or GMPXX libraries")
endif()

# Include GMP headers
include_directories(${GMP_INCLUDE_DIR})

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Add the executable for the tests
add_executable(runTests tests/ecc_tests.cpp ${SOURCES})

# Link libraries
target_link_libraries(runTests
    ${GMP_LIBRARY} 
    ${GMPXX_LIBRARY} 
    gtest gtest_main pthread
)