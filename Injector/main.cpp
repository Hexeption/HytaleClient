#include <iostream>
#include <filesystem>
#include "Injector.h"

int main(int argc, char* argv[])
{
    std::wcout << L"=====================================" << std::endl;
    std::wcout << L"    Hytale Client DLL Injector      " << std::endl;
    std::wcout << L"=====================================" << std::endl;
    std::wcout << std::endl;

    std::filesystem::path exePath = std::filesystem::current_path();
    std::filesystem::path dllPath = exePath / "Client.dll";

    if (!std::filesystem::exists(dllPath))
    {
        std::wcerr << L"Error: Client.dll not found at: " << dllPath << std::endl;
        std::wcerr << L"Please ensure Client.dll is in the same directory as the injector." << std::endl;
        std::wcout << L"\nPress Enter to exit...";
        std::cin.get();
        return 1;
    }

    std::wcout << L"DLL Path: " << dllPath << std::endl;
    std::wcout << std::endl;

    std::wstring processName = L"HytaleClient.exe";
    std::wcout << L"Target Process: " << processName << std::endl;
    std::wcout << L"Searching for process..." << std::endl;

    if (Injector::InjectDLL(processName, dllPath.wstring()))
    {
        std::wcout << L"\nSuccess! The DLL has been injected." << std::endl;
        std::wcout << L"Press END key in the game to unload the client." << std::endl;
    }
    else
    {
        std::wcerr << L"\nFailed to inject the DLL!" << std::endl;
        std::wcerr << L"Make sure:" << std::endl;
        std::wcerr << L"  1. The target process is running" << std::endl;
        std::wcerr << L"  2. You have sufficient privileges (run as administrator)" << std::endl;
        std::wcerr << L"  3. The process name is correct" << std::endl;
    }

    std::wcout << L"\nPress Enter to exit...";
    std::wcin.get();
    return 0;
}
