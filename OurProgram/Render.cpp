#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "Render.hpp"
#include "LocalPlayer.hpp"
#include "Player.hpp"
#include "Camera.hpp"
#include <d3d11.h>
#include <tchar.h>
#include <utility>
#include <cmath>

// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ImFont* regular_font;
ImFont* bold_font;
ImFont* italic_font;
ImFont* huge_font;

// Main code
void Render(LocalPlayer* Myself, std::vector<Player*>* Players, Camera* GameCamera)
{
    int width = 800;  // Define the width of the map window
    int height = 600; // Define the height of the map window

    // Create application window
    WNDCLASSEX wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ApexDMA", nullptr };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindowEx(0, wc.lpszClassName, L"ApexDMA", WS_OVERLAPPEDWINDOW, 100, 100, width, height, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup fonts
    regular_font = io.Fonts->AddFontFromFileTTF("regular.otf", 12.0f);
    bold_font = io.Fonts->AddFontFromFileTTF("bold.otf", 12.0f);
    italic_font = io.Fonts->AddFontFromFileTTF("italic.otf", 12.0f);
    huge_font = io.Fonts->AddFontFromFileTTF("bold.otf", 20.0f);

    io.FontDefault = bold_font;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

    // Define the initial scale of the map
    float mapScale = 4.0f;  // Adjust this to scale the map

    // Main loop
    while (true)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                break;
        }

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Foreground Drawlist
        ImDrawList* fg_draw = ImGui::GetForegroundDrawList();

        // Performance
        std::string PerformanceString = "Render FPS: " + std::to_string(static_cast<int>(ImGui::GetIO().Framerate));
        fg_draw->AddText(ImVec2(10, 50), IM_COL32(255, 255, 255, 255), PerformanceString.c_str());

        // Draw grid
        int gridSize = 50;
        for (int i = 0; i < width; i += gridSize)
        {
            fg_draw->AddLine(ImVec2((float)i, 0), ImVec2((float)i, (float)height), IM_COL32(255, 255, 255, 50));
        }
        for (int i = 0; i < height; i += gridSize)
        {
            fg_draw->AddLine(ImVec2(0, (float)i), ImVec2((float)width, (float)i), IM_COL32(255, 255, 255, 50));
        }

        // Map Center
        float mapCenterX = width / 2.0f;
        float mapCenterY = height / 2.0f;

        // Draw players on the map
        for (Player* player : *Players) {
            if (player->GuiState != 2 || (player->Team != 1 && player->Team != 2)) {
                continue;
            }

            // Calculate the relative position
            float relativeX = (player->Position.x - Myself->Position.x) * mapScale;
            float relativeY = (player->Position.z - Myself->Position.z) * mapScale;

            // Map the relative positions to screen coordinates
            float screenX = mapCenterX + relativeX;
            float screenY = mapCenterY - relativeY;  // Note the negative sign to flip the Y-axis

            // Draw the player position on the map
            if (player->IsAlly()) {
                fg_draw->AddCircleFilled(ImVec2(screenX, screenY), 5.0f, IM_COL32(0, 0, 255, 255));
            }
            else {
                fg_draw->AddCircleFilled(ImVec2(screenX, screenY), 5.0f, IM_COL32(255, 0, 0, 255));
            }

            // Calculate the direction the player is facing
            float yaw = player->GetYawFromRotationMatrix();
            float arrowLength = 10.0f;
            float directionX = screenX + arrowLength * cosf(yaw);
            float directionY = screenY - arrowLength * sinf(yaw);

            // Draw the arrow indicating the direction the player is facing
            fg_draw->AddLine(ImVec2(screenX, screenY), ImVec2(directionX, directionY), IM_COL32(255, 255, 255, 255), 2.0f);
        }

        // Draw local player at the center of the map
        fg_draw->AddCircleFilled(ImVec2(mapCenterX, mapCenterY), 6.0f, IM_COL32(0, 255, 0, 255));

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
