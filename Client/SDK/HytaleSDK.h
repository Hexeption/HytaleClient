#pragma once
#include "SDK.h"
#include "../Memory.h"
#include <windows.h>
#include <psapi.h>
#include <vector>

namespace SDK {

class HytaleSDK {
private:
    static HytaleSDK* instance;
    uintptr_t m_moduleBase;
    CGameContext* m_gameContext;
    bool m_initialized;

public:
    static HytaleSDK& GetInstance() {
        if (!instance)
            instance = new HytaleSDK();
        return *instance;
    }

    bool Initialize() {
        if (m_initialized)
            return true;

        MODULEINFO modInfo = Memory::GetModuleInfo("HytaleClient.exe");
        if (modInfo.lpBaseOfDll == nullptr)
            return false;

        m_moduleBase = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
        m_initialized = true;
        return true;
    }

    void SetGameContext(CGameContext* ctx) {
        m_gameContext = ctx;
    }

    uintptr_t GetModuleBase() {
        return m_moduleBase;
    }

    CGameContext* GetGameContext() {
        return m_gameContext;
    }

    CEntity* GetLocalPlayer() {
        if (!m_gameContext)
            return nullptr;
        return m_gameContext->GetLocalPlayer();
    }

    Matrix4x4* GetViewMatrix() {
        if (!m_initialized)
            return nullptr;

        uintptr_t addr = m_moduleBase + Offsets::ViewMatrixBase;
        addr = *(uintptr_t*)addr;
        if (!addr) return nullptr;

        addr = *(uintptr_t*)(addr + Offsets::ViewMatrixOffsets::Offset1);
        if (!addr) return nullptr;

        addr = *(uintptr_t*)(addr + Offsets::ViewMatrixOffsets::Offset2);
        if (!addr) return nullptr;

        addr = *(uintptr_t*)(addr + Offsets::ViewMatrixOffsets::Offset3);
        if (!addr) return nullptr;

        return (Matrix4x4*)(addr + Offsets::ViewMatrixOffsets::Final);
    }

    CEntity* GetEntityByIndex(int index) {
        if (!m_gameContext || !m_gameContext->entityList)
            return nullptr;

        return m_gameContext->entityList->GetEntityByIndex(index);
    }

    bool IsValid() {
        return m_initialized && GetLocalPlayer() != nullptr;
    }

    void EnableInfiniteStamina() {
        if (!m_initialized)
            return;

        uintptr_t addr = m_moduleBase + Offsets::InfiniteStamina;
        DWORD oldProtect;
        if (VirtualProtect((LPVOID)addr, 6, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            memset((void*)addr, 0x90, 6);
            VirtualProtect((LPVOID)addr, 6, oldProtect, &oldProtect);
        }
    }

    uintptr_t GetStaminaAddress() {
        if (!m_initialized) return 0;
        return m_moduleBase + Offsets::InfiniteStamina;
    }

    std::vector<CEntity*> GetAllPlayers(int maxScan = 2048) {
        std::vector<CEntity*> players;
        if (!m_gameContext || !m_gameContext->entityList)
            return players;

        for (int i = 0; i < maxScan; ++i) {
            CEntity* entity = GetEntityByIndex(i);
            if (entity && entity != GetLocalPlayer()) {
                players.push_back(entity);
            }
        }
        return players;
    }

private:
    HytaleSDK() : m_moduleBase(0), m_gameContext(nullptr), m_initialized(false) {}
};


}
