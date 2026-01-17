#pragma once
#include <string>

class Module
{
public:
    virtual ~Module() = default;

    virtual void OnInitialize() = 0;
    virtual void OnShutdown() = 0;

    virtual void OnUpdate()
    {
    }

    virtual std::string GetName() const = 0;
    virtual bool IsEnabled() const { return m_enabled; }

    virtual void Enable() { m_enabled = true; }
    virtual void Disable() { m_enabled = false; }

protected:
    bool m_enabled = false;
};
