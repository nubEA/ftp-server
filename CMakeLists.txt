cmake_minimum_required(VERSION 3.10) # Minimum CMake version required

# Project name and version
project(HttpServer VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)  # Use C++20 for wider compiler support
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define project structure
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# Include directories for headers
include_directories(
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/core
    ${INCLUDE_DIR}/http
    ${INCLUDE_DIR}/database
    ${INCLUDE_DIR}/handlers
    ${INCLUDE_DIR}/utility
    ${INCLUDE_DIR}/bcrypt
    ${INCLUDE_DIR}/middleware
)

# Source files
set(SOURCES
    ${SRC_DIR}/core/server.cpp
    ${SRC_DIR}/core/thread_pool.cpp
    ${SRC_DIR}/core/connection_pool.cpp
    ${SRC_DIR}/database/database.cpp
    ${SRC_DIR}/http/http_parser.cpp
    ${SRC_DIR}/http/router.cpp
    ${SRC_DIR}/http/http_response.cpp
    ${SRC_DIR}/http/http_request.cpp
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/handlers/get_register_handler.cpp
    ${SRC_DIR}/handlers/post_register_handler.cpp
    ${SRC_DIR}/utility/util.cpp
    ${SRC_DIR}/bcrypt/bcrypt.cpp
    ${SRC_DIR}/bcrypt/blowfish.cpp
    ${SRC_DIR}/handlers/get_login_handler.cpp
    ${SRC_DIR}/handlers/post_login_handler.cpp
    ${SRC_DIR}/handlers/get_home_handler.cpp
    ${SRC_DIR}/handlers/post_home_handler.cpp
    ${SRC_DIR}/middleware/auth.cpp
    ${SRC_DIR}/handlers/post_logout_handler.cpp
    ${SRC_DIR}/http/multipart_parser.cpp
    ${SRC_DIR}/handlers/get_history_handler.cpp
)

# Add the executable
add_executable(HttpServer ${SOURCES})

# Find and link PostgreSQL
find_package(PostgreSQL REQUIRED)
if(PostgreSQL_FOUND)
    message(STATUS "PostgreSQL found: ${PostgreSQL_INCLUDE_DIRS}, linking...")
    target_include_directories(HttpServer PRIVATE ${PostgreSQL_INCLUDE_DIRS})
    target_link_libraries(HttpServer PRIVATE ${PostgreSQL_LIBRARIES})
else()
    message(FATAL_ERROR "PostgreSQL NOT found. Install libpq-dev and retry.")
endif()

# Ensure OpenSSL is found and linked
find_package(OpenSSL REQUIRED)
if(OpenSSL_FOUND)
    message(STATUS "OpenSSL found and linked to the project")
    target_link_libraries(HttpServer PRIVATE OpenSSL::Crypto)
else()
    message(FATAL_ERROR "OpenSSL NOT found! Install libssl-dev (Linux) or OpenSSL (Mac/Windows).")
endif()

# Set output directory
set_target_properties(HttpServer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
)

# Compiler warnings (optional)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(HttpServer PRIVATE -Wall -Wextra -pedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(HttpServer PRIVATE /W4)
endif()
