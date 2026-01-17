cmake_minimum_required(VERSION 3.31)
project(Injector)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Injector
    main.cpp
    Injector.cpp
    Injector.h
)

set_target_properties(Injector PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

