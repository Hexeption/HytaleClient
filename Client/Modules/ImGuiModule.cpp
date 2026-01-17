#include "ImGuiModule.h"
#include "../Memory.h"
#include "../SDK/HytaleSDK.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <MinHook.h>
#include <iostream>
#include <windows.h>
#include <gl/GL.h>
#include <fstream>
#include <atomic>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef BOOL (WINAPI*SwapBuffers_t)(HDC hdc);
SwapBuffers_t ImGuiModule::oSwapBuffers = nullptr;
WNDPROC ImGuiModule::oWndProc = nullptr;
HWND ImGuiModule::g_hWnd = nullptr;
HGLRC ImGuiModule::g_hGLRC = 0;
static HDC g_hDC = NULL;
bool ImGuiModule::g_initialized = false;
bool ImGuiModule::g_showMenu = true;
std::vector<std::function<void()>> ImGuiModule::g_renderCallbacks;
static void* g_pSwapBuffersProc = nullptr;
static std::atomic_bool g_request_shutdown{false};

void RegisterImGuiModules()
{
    ImGuiModule::AddRenderCallback([]()
    {
        ImGui::Text("Hytale Client v1.0");
    });
}

void ImGuiModule::OnInitialize()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "  ImGui Module" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "[ImGui] Initializing OpenGL hooks..." << std::endl;
    RegisterImGuiModules();
    if (!InitImGui())
    {
        std::cout << "[ImGui] Failed to initialize!" << std::endl;
        return;
    }
    std::cout << "[ImGui] Successfully initialized!" << std::endl;
    std::cout << "[ImGui] Press INSERT to toggle menu" << std::endl;
}

void ImGuiModule::OnShutdown()
{
    g_request_shutdown.store(true);

    const int waitMs = 2000;
    int waited = 0;
    while (g_initialized && waited < waitMs)
    {
        Sleep(10);
        waited += 10;
    }

    if (g_initialized)
    {
        if (g_pSwapBuffersProc)
        {
            MH_DisableHook(g_pSwapBuffersProc);
            MH_RemoveHook(g_pSwapBuffersProc);
            g_pSwapBuffersProc = nullptr;
        }
        if (oWndProc && g_hWnd)
            SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)oWndProc);
        if (g_hGLRC)
        {
            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(g_hGLRC);
            g_hGLRC = NULL;
        }
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        g_hWnd = NULL;
        oWndProc = nullptr;
        g_initialized = false;
    }

    std::cout << "[ImGui] Shutdown complete" << std::endl;
}

bool ImGuiModule::InitImGui()
{
    HMODULE hGDI32 = GetModuleHandleA("gdi32.dll");
    void* pSwapBuffers = GetProcAddress(hGDI32, "SwapBuffers");
    if (MH_CreateHook(pSwapBuffers, &SwapBuffersHook, reinterpret_cast<void**>(&oSwapBuffers)) != MH_OK)
    {
        return false;
    }
    if (MH_EnableHook(pSwapBuffers) != MH_OK)
    {
        MH_RemoveHook(pSwapBuffers);
        return false;
    }
    g_pSwapBuffersProc = pSwapBuffers;
    return true;
}

void ImGuiModule::CleanupImGui()
{
    g_request_shutdown.store(true);
}

BOOL WINAPI ImGuiModule::SwapBuffersHook(HDC hdc)
{
    if (g_request_shutdown.load())
    {
        g_request_shutdown.store(false);
        if (g_hGLRC && g_hWnd)
        {
            HDC hdc_local = g_hDC ? g_hDC : GetDC(g_hWnd);
            wglMakeCurrent(hdc_local, g_hGLRC);
        }
        if (g_initialized)
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }
        if (oWndProc && g_hWnd)
            SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)oWndProc);
        if (g_hGLRC)
        {
            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(g_hGLRC);
            g_hGLRC = NULL;
        }
        g_hWnd = NULL;
        oWndProc = nullptr;
        g_initialized = false;

        if (g_pSwapBuffersProc)
        {
            MH_DisableHook(g_pSwapBuffersProc);
            MH_RemoveHook(g_pSwapBuffersProc);
            g_pSwapBuffersProc = nullptr;
        }
        if (oSwapBuffers) return oSwapBuffers(hdc);
        return ::SwapBuffers(hdc);
    }

    if (!g_initialized)
    {
        HWND hwnd = WindowFromDC(hdc);
        if (!hwnd) return oSwapBuffers ? oSwapBuffers(hdc) : ::SwapBuffers(hdc);
        int pixelFormat = GetPixelFormat(hdc);
        if (pixelFormat == 0)
        {
            PIXELFORMATDESCRIPTOR pfd = {
                sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
            };
            int pf = ChoosePixelFormat(hdc, &pfd);
            if (pf) SetPixelFormat(hdc, pf, &pfd);
        }
        HGLRC current = wglGetCurrentContext();
        if (!current)
        {
            if (!g_hGLRC)
            {
                g_hGLRC = wglCreateContext(hdc);
                if (!g_hGLRC) return oSwapBuffers ? oSwapBuffers(hdc) : ::SwapBuffers(hdc);
            }
            wglMakeCurrent(hdc, g_hGLRC);
        }
        else
        {
            g_hGLRC = current;
        }
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();
        if (!ImGui_ImplWin32_Init(hwnd)) return oSwapBuffers ? oSwapBuffers(hdc) : ::SwapBuffers(hdc);
        if (!ImGui_ImplOpenGL3_Init("#version 130")) return oSwapBuffers ? oSwapBuffers(hdc) : ::SwapBuffers(hdc);
        oWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
        g_hWnd = hwnd;
        g_hDC = hdc;
        g_initialized = true;
    }
    wglMakeCurrent(hdc, g_hGLRC);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    if (g_showMenu)
    {
        ImGui::Begin("Hytale Client", &g_showMenu, ImGuiWindowFlags_AlwaysAutoResize);
        for (auto& callback : g_renderCallbacks) callback();
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return oSwapBuffers ? oSwapBuffers(hdc) : ::SwapBuffers(hdc);
}

LRESULT CALLBACK ImGuiModule::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_KEYDOWN && wParam == VK_INSERT) g_showMenu = !g_showMenu;
    if (g_showMenu && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
    if (oWndProc) return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void ImGuiModule::AddRenderCallback(std::function<void()> callback)
{
    g_renderCallbacks.push_back(callback);
}
