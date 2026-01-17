#pragma once
#include <windows.h>
#include <atomic>
#include <thread>
#include <string>

class Client
{
public:
    static Client& GetInstance();

    bool Initialize();
    void Shutdown();

    HMODULE GetModuleHandle() const { return m_hModule; }
    void SetModuleHandle(HMODULE hModule) { m_hModule = hModule; }

private:
    Client() = default;
    ~Client() = default;

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    void UpdateLoop();

    HMODULE m_hModule = nullptr;
    bool m_initialized = false;
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_shuttingDown{false};
    std::thread m_updateThread;

    void WriteShutdownLog(const std::string& msg);

public:
    bool IsShuttingDown() const { return m_shuttingDown.load(std::memory_order_acquire); }
};
