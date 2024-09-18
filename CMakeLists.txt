cmake_minimum_required(VERSION 3.10)
project(MyTest)

# Include directories
include_directories(src)

# Find GMP library
find_path(GMP_INCLUDE_DIR NAMES gmp.h)
find_library(GMP_LIBRARY NAMES gmp)
find_library(GMPXX_LIBRARY NAMES gmpxx)
if(NOT GMP_INCLUDE_DIR OR NOT GMP_LIBRARY OR NOT GMPXX_LIBRARY)
  message(FATAL_ERROR "Could not find GMP or GMPXX libraries")
endif()
include_directories(${GMP_INCLUDE_DIR})

# Find Google Test
find_package(GTest REQUIRED)

# Add source files
file(GLOB SOURCES "src/*.cpp" "logger/*.cpp")

# Check if SYCL is enabled
option(USE_SYCL "Enable SYCL support" OFF)

if(USE_SYCL)
    # Set the icpx compiler with SYCL support
    set(CMAKE_CXX_COMPILER icpx)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsycl -g")
    # Add oneAPI include directories
    include_directories(${CMAKE_SOURCE_DIR}/include /opt/intel/oneapi/compiler/latest/linux/include)
    # Add oneAPI library directories
    link_directories(/opt/intel/oneapi/compiler/latest/linux/lib)
    message(STATUS "Compiling with SYCL support")
    add_definitions(-DUSE_SYCL)
else()
    message(STATUS "Compiling without SYCL support")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    remove_definitions(-DUSE_SYCL)
endif()

# Set build type to Debug
set(CMAKE_BUILD_TYPE Debug)

# Add the executable for the tests
add_executable(runTests tests/aes_tests.cpp ${SOURCES})

# Link libraries
target_link_libraries(runTests
    ${GMP_LIBRARY}
    ${GMPXX_LIBRARY}
    gtest
    gtest_main
    pthread
)

# Optional: Output directory for the executable
set_target_properties(runTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
