cmake_minimum_required(VERSION 3.10)
project(MyTest)
# Include directories
include_directories(src)

# Find Google Test
find_package(GTest REQUIRED)
# Add source files
file(GLOB SOURCES "src/*.cpp")
# Check if SYCL is enabled
option(USE_SYCL "Enable SYCL support" ON)
if(USE_SYCL)
    # Set the icpx compiler with SYCL support
    set(CMAKE_CXX_COMPILER icpx)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsycl")
    # Add oneAPI include directories
    include_directories(${CMAKE_SOURCE_DIR}/include /opt/intel/oneapi/compiler/latest/linux/include)
    # Add oneAPI library directories
    link_directories(/opt/intel/oneapi/compiler/latest/linux/lib)
    message(STATUS "Compiling with SYCL support")
    add_definitions(-DUSE_SYCL)
    else()
    message(STATUS "Compiling without SYCL support")
    remove_definitions(-DUSE_SYCL)
endif()
# Add the executable for the tests
add_executable(runTests tests/aes_tests.cpp ${SOURCES})
# Link libraries
target_link_libraries(runTests
    GTest::GTest
    GTest::Main
    pthread
)
# Enable testing
enable_testing()
gtest_discover_tests(runTests)