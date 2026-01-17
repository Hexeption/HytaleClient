#pragma once
#include <windows.h>
#include <string>

class Injector
{
public:
    static bool InjectDLL(DWORD processId, const std::wstring& dllPath);
    static bool InjectDLL(const std::wstring& processName, const std::wstring& dllPath);
    static DWORD GetProcessIdByName(const std::wstring& processName);
    static bool IsProcessRunning(DWORD processId);
    static std::wstring GetLastErrorAsString();
};
