#pragma once
#include "../Module.h"
class TeleportModule : public Module {
public:
    void OnInitialize() override;
    void OnShutdown() override;
    std::string GetName() const override { return "Teleport"; }
};

