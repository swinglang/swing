cmake_minimum_required(VERSION 3.15)
project(WheeProject LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings (optional but useful)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif (MSVC)
    add_compile_options(/W4)
endif()

# Enable multithreading support
find_package(Threads REQUIRED)

# Define source files
add_executable(wheec src/wheec.cpp)
target_link_libraries(wheec PRIVATE Threads::Threads)

add_executable(whee src/whee.cpp)
target_link_libraries(whee PRIVATE Threads::Threads)

add_executable(wcc src/wcc.cpp)
target_link_libraries(wcc PRIVATE Threads::Threads)
