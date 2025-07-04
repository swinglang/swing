cmake_minimum_required(VERSION 3.15)
project(WheeProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# You can add extra flags if needed:
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Find Threads (for multithreading support)
find_package(Threads REQUIRED)

add_executable(wheec src/wheec.cpp)
target_link_libraries(wheec PRIVATE Threads::Threads)

add_executable(whee src/whee.cpp)
target_link_libraries(whee PRIVATE Threads::Threads)

add_executable(wcc src/wcc.cpp)
target_link_libraries(wcc PRIVATE Threads::Threads)
