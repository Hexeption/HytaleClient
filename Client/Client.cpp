#include "Client.h"
#include "ModuleManager.h"
#include "Modules/ConsoleModule.h"
#include "Modules/GameTickModule.h"
#include "Modules/ImGuiModule.h"
#include "Modules/StaminaModule.h"
#include "Modules/TeleportModule.h"
#include "Modules/PlayerInfoModule.h"
#include <MinHook.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>


Client& Client::GetInstance()
{
    static Client instance;
    return instance;
}

bool Client::Initialize()
{
    if (m_initialized)
    {
        return true;
    }

    if (MH_Initialize() != MH_OK)
    {
        MessageBoxA(nullptr, "Failed to initialize MinHook!", "Hytale Client Error", MB_OK | MB_ICONERROR);
        return false;
    }

    ModuleManager::GetInstance().RegisterModule(std::make_unique<ConsoleModule>());
    ModuleManager::GetInstance().RegisterModule(std::make_unique<GameTickModule>());
    ModuleManager::GetInstance().RegisterModule(std::make_unique<ImGuiModule>());
    ModuleManager::GetInstance().RegisterModule(std::make_unique<PlayerInfoModule>());
    ModuleManager::GetInstance().RegisterModule(std::make_unique<TeleportModule>());
    ModuleManager::GetInstance().RegisterModule(std::make_unique<StaminaModule>());

    ModuleManager::GetInstance().Initialize();

    std::cout << "[Client] Hytale Client initialized successfully!" << std::endl;
    std::cout << "[Client] Modules loaded: " << ModuleManager::GetInstance().GetModules().size() << std::endl;

    m_running = true;
    m_updateThread = std::thread(&Client::UpdateLoop, this);

    m_initialized = true;
    return true;
}

void Client::Shutdown()
{
    if (!m_initialized)
    {
        return;
    }

    std::cout << "[Client] Shutting down Hytale Client..." << std::endl;

    m_running = false;
    if (m_updateThread.joinable())
    {
        m_updateThread.join();
    }

    ModuleManager::GetInstance().Shutdown();

    MH_Uninitialize();

    m_initialized = false;
}

void Client::UpdateLoop()
{
    std::cout << "[Client] Update thread started" << std::endl;

    while (m_running)
    {
        ModuleManager::GetInstance().Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    std::cout << "[Client] Update thread stopped" << std::endl;
}
