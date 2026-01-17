cmake_minimum_required(VERSION 3.31)
project(HytaleClient)

# Using C++20 (MSVC 19.45 doesn't fully support C++26 yet)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Client)
add_subdirectory(Injector)


