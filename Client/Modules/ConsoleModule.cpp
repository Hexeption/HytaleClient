#include "ConsoleModule.h"
#include "ImGuiModule.h"
#include <windows.h>
#include <iostream>
#include <imgui.h>

void ConsoleModule::OnInitialize()
{
    if (AllocConsole())
    {
        m_consoleAllocated = true;

        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        freopen_s(&fp, "CONIN$", "r", stdin);

        std::cout.clear();
        std::cerr.clear();
        std::cin.clear();

        SetConsoleTitleA("Hytale Client - Console");

        std::cout << "==================================" << std::endl;
        std::cout << "  Hytale Client Console Module   " << std::endl;
        std::cout << "==================================" << std::endl;
        std::cout << "Console initialized successfully!" << std::endl;
        std::cout << std::endl;
    }
}

void ConsoleModule::OnShutdown()
{
    if (m_consoleAllocated)
    {
        std::cout << "Console module shutting down..." << std::endl;

        HWND consoleWindow = GetConsoleWindow();
        if (consoleWindow != NULL)
        {
            ShowWindow(consoleWindow, SW_HIDE);
            PostMessage(consoleWindow, WM_CLOSE, 0, 0);
        }

        FreeConsole();
        m_consoleAllocated = false;
    }
}
