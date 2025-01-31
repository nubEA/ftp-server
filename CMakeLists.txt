cmake_minimum_required(VERSION 3.10) # Minimum CMake version required

# Project name and version
project(URLShortner VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)  # Use C++20 instead of C++23 for better compiler support
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define project structure
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# Include directories
include_directories(${INCLUDE_DIR})

# Source files
set(SOURCES
    ${SRC_DIR}/server.cpp
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/thread_pool.cpp
    ${SRC_DIR}/database.cpp
    ${SRC_DIR}/connection_pool.cpp
)

# Add the executable
add_executable(URLShortner ${SOURCES})

# Find and link PostgreSQL
find_package(PostgreSQL REQUIRED)

if(PostgreSQL_FOUND)
    message(STATUS "PostgreSQL found: ${PostgreSQL_INCLUDE_DIRS}, linking...")
    target_include_directories(URLShortner PRIVATE ${PostgreSQL_INCLUDE_DIRS})
    target_link_libraries(URLShortner PRIVATE ${PostgreSQL_LIBRARIES})
else()
    message(FATAL_ERROR "PostgreSQL NOT found. Install libpq-dev and retry.")
endif()

# Ensure OpenSSL is found and linked
find_package(OpenSSL REQUIRED)
if(OpenSSL_FOUND)
    message(STATUS "OpenSSL found and linked to the project")
    target_link_libraries(URLShortner PRIVATE OpenSSL::Crypto)
else()
    message(FATAL_ERROR "OpenSSL NOT found! Install libssl-dev (Linux) or OpenSSL (Mac/Windows).")
endif()

# Set output directory
set_target_properties(URLShortner PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
)

# Compiler warnings (optional)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(URLShortner PRIVATE -Wall -Wextra -pedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(URLShortner PRIVATE /W4)
endif()
