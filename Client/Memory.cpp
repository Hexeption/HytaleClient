#include "Memory.h"
#include <psapi.h>
#include <fstream>
#include <iostream>

#pragma comment(lib, "psapi.lib")

uintptr_t Memory::PatternScan(const char* moduleName, const char* pattern, const char* mask)
{
    MODULEINFO modInfo = GetModuleInfo(moduleName);
    if (modInfo.lpBaseOfDll == nullptr)
    {
        return 0;
    }

    return PatternScan(reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll), modInfo.SizeOfImage, pattern, mask);
}

uintptr_t Memory::PatternScan(uintptr_t start, size_t size, const char* pattern, const char* mask)
{
    size_t patternLength = strlen(mask);

    for (size_t i = 0; i < size - patternLength; i++)
    {
        bool found = true;
        for (size_t j = 0; j < patternLength; j++)
        {
            if (mask[j] != '?' && pattern[j] != *reinterpret_cast<char*>(start + i + j))
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            return start + i;
        }
    }

    return 0;
}

std::vector<uintptr_t> Memory::PatternScanAll(const char* moduleName, const char* pattern, const char* mask)
{
    std::vector<uintptr_t> results;
    MODULEINFO modInfo = GetModuleInfo(moduleName);
    if (modInfo.lpBaseOfDll == nullptr)
    {
        return results;
    }

    uintptr_t start = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
    size_t size = modInfo.SizeOfImage;
    size_t patternLength = strlen(mask);

    for (size_t i = 0; i < size - patternLength; i++)
    {
        bool found = true;
        for (size_t j = 0; j < patternLength; j++)
        {
            if (mask[j] != '?' && pattern[j] != *reinterpret_cast<char*>(start + i + j))
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            results.push_back(start + i);
        }
    }

    return results;
}

uintptr_t Memory::GetModuleBase(const char* moduleName)
{
    return reinterpret_cast<uintptr_t>(GetModuleHandleA(moduleName));
}

size_t Memory::GetModuleSize(const char* moduleName)
{
    ::MODULEINFO modInfo = GetModuleInfo(moduleName);
    return modInfo.SizeOfImage;
}

::MODULEINFO Memory::GetModuleInfo(const char* moduleName)
{
    ::MODULEINFO modInfo = {0};
    HMODULE hModule = GetModuleHandleA(moduleName);

    if (hModule)
    {
        GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));
    }

    return modInfo;
}

uintptr_t Memory::GetAbsoluteAddress(uintptr_t instructionPtr, int offset, int size)
{
    return instructionPtr + *reinterpret_cast<int*>(instructionPtr + offset) + size;
}

uintptr_t Memory::GetRelativeAddress(uintptr_t from, uintptr_t to, int instructionSize)
{
    return to - from - instructionSize;
}

bool Memory::DumpMemoryRegion(uintptr_t address, size_t size, const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for dumping: " << filename << std::endl;
        return false;
    }

    try
    {
        file.write(reinterpret_cast<const char*>(address), size);
        file.close();
        std::cout << "Successfully dumped " << size << " bytes to " << filename << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to dump memory: " << e.what() << std::endl;
        return false;
    }
}

bool Memory::DumpModule(const char* moduleName, const std::string& filename)
{
    ::MODULEINFO modInfo = GetModuleInfo(moduleName);
    if (modInfo.lpBaseOfDll == nullptr)
    {
        std::cerr << "Module not found: " << moduleName << std::endl;
        return false;
    }

    std::cout << "Dumping module: " << moduleName << std::endl;
    std::cout << "  Base: 0x" << std::hex << reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll) << std::endl;
    std::cout << "  Size: 0x" << std::hex << modInfo.SizeOfImage << std::dec << " bytes" << std::endl;

    return DumpMemoryRegion(reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll), modInfo.SizeOfImage, filename);
}

std::vector<uintptr_t> Memory::FindString(const char* moduleName, const std::string& str)
{
    std::vector<uintptr_t> results;
    MODULEINFO modInfo = GetModuleInfo(moduleName);
    if (modInfo.lpBaseOfDll == nullptr)
    {
        return results;
    }

    uintptr_t start = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
    size_t size = modInfo.SizeOfImage;

    for (size_t i = 0; i < size - str.length(); i++)
    {
        if (memcmp(reinterpret_cast<void*>(start + i), str.c_str(), str.length()) == 0)
        {
            results.push_back(start + i);
        }
    }

    return results;
}

std::vector<uintptr_t> Memory::FindStringReferences(const char* moduleName, const std::string& str)
{
    std::vector<uintptr_t> results;

    std::vector<uintptr_t> stringAddresses = FindString(moduleName, str);
    if (stringAddresses.empty())
    {
        return results;
    }

    MODULEINFO modInfo = GetModuleInfo(moduleName);
    if (modInfo.lpBaseOfDll == nullptr)
    {
        return results;
    }

    uintptr_t start = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
    size_t size = modInfo.SizeOfImage;

    for (uintptr_t stringAddr : stringAddresses)
    {
        for (size_t i = 0; i < size - sizeof(uintptr_t); i++)
        {
            uintptr_t* ptr = reinterpret_cast<uintptr_t*>(start + i);
            if (*ptr == stringAddr)
            {
                results.push_back(start + i);
            }
        }
    }

    return results;
}
