#pragma once
#include "../Module.h"
#include "../SDK/HytaleSDK.h"
#include <string>
#include <windows.h>
#include <MinHook.h>

typedef __int64(__fastcall* GameTickType)(__int64 instance, __int64 a2);

class GameTickModule : public Module {
public:
    GameTickModule() = default;
    ~GameTickModule() override = default;

    void OnInitialize() override;
    void OnShutdown() override;
    std::string GetName() const override { return "GameTick"; }

private:
    bool m_hooked = false;
    void* m_originalGameTick = nullptr;



    static __int64 __fastcall GameTickHook(__int64 instance, __int64 a2);
    bool FindAndHookGameTick();
};

extern class GameTickModule* g_GameTickModule;
