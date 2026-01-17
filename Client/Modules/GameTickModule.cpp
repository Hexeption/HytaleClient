#include "GameTickModule.h"
#include "../Memory.h"
#include <iostream>

static GameTickType g_OriginalGameTick = nullptr;
GameTickModule* g_GameTickModule = nullptr;

void GameTickModule::OnInitialize() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Game Tick Hook Module" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "[GameTick] Initializing..." << std::endl;

    SDK::HytaleSDK::GetInstance().Initialize();

    if (FindAndHookGameTick()) {
        std::cout << "[GameTick] Successfully hooked game tick!" << std::endl;
        m_hooked = true;
    } else {
        std::cout << "[GameTick] Failed to hook game tick" << std::endl;
    }
}

void GameTickModule::OnShutdown() {
    if (m_hooked && m_originalGameTick) {
        MH_DisableHook(m_originalGameTick);
        std::cout << "[GameTick] Unhooked game tick" << std::endl;
    }
}

bool GameTickModule::FindAndHookGameTick() {
    const char* pattern = "\x55\x41\x57\x41\x56\x41\x55\x41\x54\x57\x56\x53\x48\x81\xEC\x00\x00\x00\x00\x0F\x29\xB4\x24\x00\x00\x00\x00\x0F\x29\xBC\x24\x00\x00\x00\x00\x44\x0F\x29\x84\x24\x00\x00\x00\x00\x48\x8D\xAC\x24\x00\x00\x00\x00\x33\xC0\x48\x89\x85\x00\x00\x00\x00\x0F\x57\xE4\x48\xB8";
    const char* mask = "xxxxxxxxxxxxxxx????xxxx????xxxx????xxxxx????xxxx????xxxxx????xxxx";

    uintptr_t funcAddr = Memory::PatternScan("HytaleClient.exe", pattern, mask);

    if (!funcAddr) {
        std::cout << "[GameTick] Pattern not found!" << std::endl;
        return false;
    }

    std::cout << "[GameTick] Found GameTick at: 0x" << std::hex << funcAddr << std::dec << std::endl;

    m_originalGameTick = (void*)funcAddr;

    if (MH_CreateHook((LPVOID)funcAddr, &GameTickHook, reinterpret_cast<LPVOID*>(&g_OriginalGameTick)) != MH_OK) {
        std::cout << "[GameTick] Failed to create hook!" << std::endl;
        return false;
    }

    if (MH_EnableHook((LPVOID)funcAddr) != MH_OK) {
        std::cout << "[GameTick] Failed to enable hook!" << std::endl;
        return false;
    }

    return true;
}

__int64 __fastcall GameTickModule::GameTickHook(__int64 instance, __int64 a2) {
    static int tickCounter = 0;
    tickCounter++;

    SDK::HytaleSDK& sdk = SDK::HytaleSDK::GetInstance();
    sdk.SetGameContext((SDK::CGameContext*)instance);

    if (tickCounter % 60 == 0) {
        SDK::CEntity* localPlayer = sdk.GetLocalPlayer();

        if (localPlayer) {
            SDK::Vector3 pos = localPlayer->GetPosition();
            SDK::Rotation rot = localPlayer->GetRotation();

            std::cout << "[GameTick] Tick #" << tickCounter << std::endl;
            std::cout << "  Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
            std::cout << "  Rotation: (Yaw=" << rot.yaw << ", Pitch=" << rot.pitch << ", Roll=" << rot.roll << ")" << std::endl;
        }
    }

    return g_OriginalGameTick(instance, a2);
}
