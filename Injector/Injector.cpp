#include "Injector.h"
#include <tlhelp32.h>
#include <iostream>

bool Injector::InjectDLL(DWORD processId, const std::wstring& dllPath)
{
    if (!IsProcessRunning(processId))
    {
        std::wcerr << L"Process is not running!" << std::endl;
        return false;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProcess)
    {
        std::wcerr << L"Failed to open process: " << GetLastErrorAsString() << std::endl;
        return false;
    }

    SIZE_T dllPathSize = (dllPath.length() + 1) * sizeof(wchar_t);
    LPVOID pDllPath = VirtualAllocEx(hProcess, nullptr, dllPathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!pDllPath)
    {
        std::wcerr << L"Failed to allocate memory: " << GetLastErrorAsString() << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    if (!WriteProcessMemory(hProcess, pDllPath, dllPath.c_str(), dllPathSize, nullptr))
    {
        std::wcerr << L"Failed to write process memory: " << GetLastErrorAsString() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32)
    {
        std::wcerr << L"Failed to get kernel32.dll handle!" << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    LPVOID pLoadLibraryW = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryW");
    if (!pLoadLibraryW)
    {
        std::wcerr << L"Failed to get LoadLibraryW address!" << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
                                        (LPTHREAD_START_ROUTINE)pLoadLibraryW, pDllPath, 0, nullptr);

    if (!hThread)
    {
        std::wcerr << L"Failed to create remote thread: " << GetLastErrorAsString() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeThread(hThread, &exitCode);

    VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    if (exitCode == 0)
    {
        std::wcerr << L"LoadLibraryW failed in target process!" << std::endl;
        return false;
    }

    std::wcout << L"DLL injected successfully!" << std::endl;
    return true;
}

bool Injector::InjectDLL(const std::wstring& processName, const std::wstring& dllPath)
{
    DWORD processId = GetProcessIdByName(processName);
    if (processId == 0)
    {
        std::wcerr << L"Process not found: " << processName << std::endl;
        return false;
    }

    std::wcout << L"Found process: " << processName << L" (PID: " << processId << L")" << std::endl;
    return InjectDLL(processId, dllPath);
}

DWORD Injector::GetProcessIdByName(const std::wstring& processName)
{
    DWORD processId = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe32))
    {
        do
        {
            if (_wcsicmp(pe32.szExeFile, processName.c_str()) == 0)
            {
                processId = pe32.th32ProcessID;
                break;
            }
        }
        while (Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return processId;
}

bool Injector::IsProcessRunning(DWORD processId)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (!hProcess)
    {
        return false;
    }

    DWORD exitCode = 0;
    GetExitCodeProcess(hProcess, &exitCode);
    CloseHandle(hProcess);

    return exitCode == STILL_ACTIVE;
}

std::wstring Injector::GetLastErrorAsString()
{
    DWORD errorId = GetLastError();
    if (errorId == 0)
    {
        return L"No error";
    }

    LPWSTR messageBuffer = nullptr;
    size_t size = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, errorId, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&messageBuffer, 0, nullptr
    );

    std::wstring message(messageBuffer, size);
    LocalFree(messageBuffer);

    return message;
}
