#include "TeleportModule.h"
#include "ImGuiModule.h"
#include "../SDK/HytaleSDK.h"
#include <imgui.h>

void TeleportModule::OnInitialize() {
    ImGuiModule::AddRenderCallback([]() {
        SDK::HytaleSDK& sdk = SDK::HytaleSDK::GetInstance();
        SDK::CEntity* player = sdk.GetLocalPlayer();
        if (player) {
            SDK::Vector3 pos = player->GetPosition();
            if (ImGui::Button("Teleport Up (+10)")) { pos.y += 10.0f; player->SetPosition(pos); }
            ImGui::SameLine();
            if (ImGui::Button("Teleport Down (-10)")) { pos.y -= 10.0f; player->SetPosition(pos); }
            static float teleportX = 0.0f, teleportY = 70.0f, teleportZ = 0.0f;
            ImGui::InputFloat("X", &teleportX);
            ImGui::InputFloat("Y", &teleportY);
            ImGui::InputFloat("Z", &teleportZ);
            if (ImGui::Button("Teleport to Position")) player->SetPosition(SDK::Vector3(teleportX, teleportY, teleportZ));
        }
    });
}

void TeleportModule::OnShutdown() {
}

