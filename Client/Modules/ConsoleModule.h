#pragma once
#include "../Module.h"

class ConsoleModule : public Module
{
public:
    ConsoleModule() = default;
    ~ConsoleModule() override = default;

    void OnInitialize() override;
    void OnShutdown() override;
    std::string GetName() const override { return "Console"; }

private:
    bool m_consoleAllocated = false;
};
