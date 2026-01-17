#include "PlayerInfoModule.h"
#include "ImGuiModule.h"
#include "../SDK/HytaleSDK.h"
#include <imgui.h>

void PlayerInfoModule::OnInitialize() {
    ImGuiModule::AddRenderCallback([]() {
        SDK::HytaleSDK& sdk = SDK::HytaleSDK::GetInstance();
        SDK::CEntity* player = sdk.GetLocalPlayer();
        if (player) {
            SDK::Vector3 pos = player->GetPosition();
            SDK::Rotation rot = player->GetRotation();
            ImGui::Text("Position: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
            ImGui::Text("Rotation: %.2f, %.2f, %.2f", rot.yaw, rot.pitch, rot.roll);
        } else {
            ImGui::Text("Player: Not Found");
        }
    });
}

void PlayerInfoModule::OnShutdown() {
}

