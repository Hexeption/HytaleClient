#pragma once
#include <string>
#include "../Module.h"

class StaminaModule : public Module {
public:
    std::string GetName() const override { return "Stamina"; }
    void OnShutdown() override;
    void OnInitialize() override;
};
