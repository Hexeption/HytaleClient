#pragma once
#include <windows.h>
#include <psapi.h>
#include <string>
#include <vector>
#include <optional>


class Memory
{
public:
    static uintptr_t PatternScan(const char* moduleName, const char* pattern, const char* mask);
    static uintptr_t PatternScan(uintptr_t start, size_t size, const char* pattern, const char* mask);
    static std::vector<uintptr_t> PatternScanAll(const char* moduleName, const char* pattern, const char* mask);

    static uintptr_t GetModuleBase(const char* moduleName);
    static size_t GetModuleSize(const char* moduleName);
    static MODULEINFO GetModuleInfo(const char* moduleName);

    template <typename T>
    static T Read(uintptr_t address)
    {
        return *reinterpret_cast<T*>(address);
    }

    template <typename T>
    static void Write(uintptr_t address, T value)
    {
        DWORD oldProtect;
        VirtualProtect(reinterpret_cast<void*>(address), sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect);
        *reinterpret_cast<T*>(address) = value;
        VirtualProtect(reinterpret_cast<void*>(address), sizeof(T), oldProtect, &oldProtect);
    }

    static uintptr_t GetAbsoluteAddress(uintptr_t instructionPtr, int offset, int size);
    static uintptr_t GetRelativeAddress(uintptr_t from, uintptr_t to, int instructionSize);

    static bool DumpMemoryRegion(uintptr_t address, size_t size, const std::string& filename);
    static bool DumpModule(const char* moduleName, const std::string& filename);

    static std::vector<uintptr_t> FindString(const char* moduleName, const std::string& str);
    static std::vector<uintptr_t> FindStringReferences(const char* moduleName, const std::string& str);
};
