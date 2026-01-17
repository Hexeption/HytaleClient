cmake_minimum_required(VERSION 3.31)
project(Client)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        minhook
        GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
        GIT_TAG master
)
FetchContent_MakeAvailable(minhook)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

set(IMGUI_DIR ${imgui_SOURCE_DIR})

set(IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(Client SHARED
        dllmain.cpp
        Client.cpp
        Client.h
        ModuleManager.cpp
        ModuleManager.h
        Module.h
        Memory.cpp
        Memory.h
        SDK/SDK.h
        SDK/HytaleSDK.h
        SDK/HytaleSDK.cpp
        Modules/ConsoleModule.cpp
        Modules/ConsoleModule.h
        Modules/GameTickModule.cpp
        Modules/GameTickModule.h
        Modules/ImGuiModule.cpp
        Modules/ImGuiModule.h
        Modules/PlayerInfoModule.cpp
        Modules/PlayerInfoModule.h
        Modules/StaminaModule.cpp
        Modules/StaminaModule.h
        Modules/TeleportModule.cpp
        Modules/TeleportModule.h
        ${IMGUI_SOURCES}
)

target_include_directories(Client PRIVATE
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)

target_link_libraries(Client PRIVATE minhook opengl32)

set_target_properties(Client PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
