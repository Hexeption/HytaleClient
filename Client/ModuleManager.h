#pragma once
#include <vector>
#include <memory>
#include "Module.h"

class ModuleManager
{
public:
    static ModuleManager& GetInstance();

    void Initialize();
    void Shutdown();
    void Update();

    void RegisterModule(std::unique_ptr<Module> module);
    Module* GetModule(const std::string& name);

    const std::vector<std::unique_ptr<Module>>& GetModules() const { return m_modules; }

private:
    ModuleManager() = default;
    ~ModuleManager() = default;

    ModuleManager(const ModuleManager&) = delete;
    ModuleManager& operator=(const ModuleManager&) = delete;

    std::vector<std::unique_ptr<Module>> m_modules;
};
