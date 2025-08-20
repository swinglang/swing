cmake_minimum_required(VERSION 3.15)
project(WheeProject LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Enable warnings
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif (MSVC)
    add_compile_options(/W4)
endif()

# Enable multithreading support if needed
find_package(Threads REQUIRED)

# Gather sources
file(GLOB_RECURSE SOURCES
    src/*.c
    src/cmd/*.c
)

# Define single binary
add_executable(swingc ${SOURCES})

# Link with pthreads if available
target_link_libraries(wheec PRIVATE Threads::Threads)
