#pragma once
#include "../Module.h"
#include <functional>
#include <vector>
#include <windows.h>
#include <gl/GL.h>

typedef BOOL(WINAPI* SwapBuffers_t)(HDC hdc);

class ImGuiModule : public Module {
public:
    ImGuiModule() = default;
    ~ImGuiModule() override = default;

    void OnInitialize() override;
    void OnShutdown() override;
    std::string GetName() const override { return "ImGui"; }

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static void AddRenderCallback(std::function<void()> callback);

private:
    bool InitImGui();
    void CleanupImGui();
    static BOOL WINAPI SwapBuffersHook(HDC hdc);
    static bool InitDeviceAndContext(HDC hdc);

    static SwapBuffers_t oSwapBuffers;
    static WNDPROC oWndProc;

    static HWND g_hWnd;
    static HGLRC g_hGLRC;
    static bool g_initialized;
    static bool g_showMenu;

    static std::vector<std::function<void()>> g_renderCallbacks;
};
