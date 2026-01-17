#include "StaminaModule.h"
#include "ImGuiModule.h"
#include "../SDK/HytaleSDK.h"
#include <imgui.h>
#include <windows.h>

void StaminaModule::OnInitialize()
{
    ImGuiModule::AddRenderCallback([]()
    {
        SDK::HytaleSDK& sdk = SDK::HytaleSDK::GetInstance();
        SDK::CEntity* player = sdk.GetLocalPlayer();
        if (player)
        {
            static bool staminaEnabled = false;
            static BYTE originalStaminaBytes[6] = {0};
            static bool staminaPatched = false;
            uintptr_t staminaAddr = sdk.GetStaminaAddress();
            if (staminaAddr)
            {
                ImGui::Separator();
                if (!staminaPatched)
                {
                    memcpy(originalStaminaBytes, (void*)staminaAddr, 6);
                }
                if (ImGui::Checkbox("Infinite Stamina", &staminaEnabled))
                {
                    if (staminaEnabled)
                    {
                        BYTE patch[6] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
                        DWORD oldProtect;
                        VirtualProtect((LPVOID)staminaAddr, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
                        memcpy((void*)staminaAddr, patch, 6);
                        VirtualProtect((LPVOID)staminaAddr, 6, oldProtect, &oldProtect);
                        staminaPatched = true;
                    }
                    else
                    {
                        DWORD oldProtect;
                        VirtualProtect((LPVOID)staminaAddr, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
                        memcpy((void*)staminaAddr, originalStaminaBytes, 6);
                        VirtualProtect((LPVOID)staminaAddr, 6, oldProtect, &oldProtect);
                        staminaPatched = false;
                    }
                }
            }
        }
    });
}

void StaminaModule::OnShutdown()
{
}
