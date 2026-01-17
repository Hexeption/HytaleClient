#include "ModuleManager.h"
#include <algorithm>

ModuleManager& ModuleManager::GetInstance()
{
    static ModuleManager instance;
    return instance;
}

void ModuleManager::Initialize()
{
    for (auto& module : m_modules)
    {
        module->OnInitialize();
        module->Enable();
    }
}

void ModuleManager::Shutdown()
{
    for (auto& module : m_modules)
    {
        module->Disable();
        module->OnShutdown();
    }
    m_modules.clear();
}

void ModuleManager::RegisterModule(std::unique_ptr<Module> module)
{
    if (module)
    {
        m_modules.push_back(std::move(module));
    }
}

Module* ModuleManager::GetModule(const std::string& name)
{
    auto it = std::find_if(m_modules.begin(), m_modules.end(),
                           [&name](const std::unique_ptr<Module>& module)
                           {
                               return module->GetName() == name;
                           });

    return it != m_modules.end() ? it->get() : nullptr;
}

void ModuleManager::Update()
{
    for (auto& module : m_modules)
    {
        if (module->IsEnabled())
        {
            module->OnUpdate();
        }
    }
}
