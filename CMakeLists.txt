cmake_minimum_required(VERSION 3.8)

project("CMakeProject" VERSION 1.0.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories("src")
include_directories("Include")

# Glob source files
file(GLOB Q_FILES "src/*.*")
file(GLOB ANS_FILES "./*.cpp")
set(SOURCE_FILES "")
list(APPEND SOURCE_FILES ${Q_FILES} ${ANS_FILES})

# Add library
add_library(Input src/input.cpp "src/full_condition.cpp" "src/and_operator.h" "src/operator_node.h" "src/operator_node.cpp" "src/full_condition.h" "src/basic_condition.h" "src/basic_condition.cpp" "src/or_operator.h" "src/or_operator.cpp"   )

# Add executable
add_executable(CMakeProject ${SOURCE_FILES} "src/full_condition.cpp" "src/and_operator.h" "src/operator_node.h" "src/operator_node.cpp" "src/full_condition.h" "src/basic_condition.h" "src/basic_condition.cpp" "src/or_operator.h" "src/or_operator.cpp"   )

target_link_libraries(CMakeProject PRIVATE Input)

# Add GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

# Prevent GoogleTest from overriding our compiler/linker options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Test files
file(GLOB TEST_FILES "Tests/*.cpp")

# Add test executable
add_executable(RunTests ${TEST_FILES}  )

# Link GTest and project library
target_link_libraries(RunTests gtest gtest_main Input ws2_32 msvcprtd)

# Set runtime library to be the same for all targets
if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MD")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MD")
endif()

# Enable testing
enable_testing()

# Add tests
add_test(NAME RunTests COMMAND RunTests)

if (UNIX AND NOT APPLE)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-incompatible-pointer-types)
endif()

if (UNIX)
    include(CheckCCompilerFlag)
    CHECK_C_COMPILER_FLAG("-fsanitize=address" COMPILER_SUPPORTS_ASAN)
    if(COMPILER_SUPPORTS_ASAN)
        target_compile_options(CMakeProject PUBLIC -fsanitize=address)
        target_link_libraries(CMakeProject -fsanitize=address)
    endif()
endif()

# Copy json_to_read.json to the build directory
configure_file(${CMAKE_SOURCE_DIR}/json_to_read.json ${CMAKE_BINARY_DIR}/json_to_read.json COPYONLY)
