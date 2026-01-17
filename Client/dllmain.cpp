#include <windows.h>
#include <thread>
#include "Client.h"

DWORD WINAPI MainThread(LPVOID lpParam)
{
    if (!Client::GetInstance().Initialize())
    {
        FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), 0);
        return 0;
    }

    while (true)
    {
        if (GetAsyncKeyState(VK_END) & 1)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    Client::GetInstance().Shutdown();

    FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        Client::GetInstance().SetModuleHandle(hModule);

        if (HANDLE hThread = CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr))
        {
            CloseHandle(hThread);
        }
        break;

    case DLL_PROCESS_DETACH:
        Client::GetInstance().Shutdown();
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
