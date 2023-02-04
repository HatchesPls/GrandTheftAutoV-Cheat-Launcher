#include "directx.hpp"
#include "../app/version.hpp"
#include "../app/filesystem.hpp"
#include "../app/network.hpp"
#include "../cheat/cheat.hpp"
#include "../cheat/module_inject.hpp"
#include "../app/log.hpp"
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>
#include <string>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace app
{ 
    HWND directx::app_window                                = NULL;
    WNDCLASSEX directx::w_class;
    ID3D11Device* directx::g_pd3dDevice                     = NULL;
    IDXGISwapChain* directx::g_pSwapChain                   = NULL;
    ID3D11DeviceContext* directx::g_pd3dDeviceContext       = NULL;
    ID3D11RenderTargetView* directx::g_mainRenderTargetView = NULL;

    struct directx::imgui_notify notification;

    void directx::cleanup_render_target()
    {
        if (g_mainRenderTargetView)
        {
            g_mainRenderTargetView->Release();
            g_mainRenderTargetView = NULL;
        }
    }
    void directx::create_render_target()
    {
        ID3D11Texture2D* pBackBuffer;
        g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (pBackBuffer)
        {
            g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
            pBackBuffer->Release();
        }
    }
    bool directx::create_swapchain()
    {
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
        sd.OutputWindow = app_window;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL FeatureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
        if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, FeatureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
            return false;

        create_render_target();
        return true;
    }
    void directx::delete_swapchain()
    {
        cleanup_render_target();

        if (g_pSwapChain) 
        { 
            g_pSwapChain->Release(); 
            g_pSwapChain = NULL; 
        }

        if (g_pd3dDeviceContext) 
        { 
            g_pd3dDeviceContext->Release(); 
            g_pd3dDeviceContext = NULL; 
        }

        if (g_pd3dDevice) 
        { 
            g_pd3dDevice->Release(); 
            g_pd3dDevice = NULL; 
        }
    }
	LRESULT WINAPI directx::wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) { return true; }

        switch (msg)
        {
        case WM_SIZE:
            if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
            {
                cleanup_render_target();
                g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                create_render_target();
            }
            return 0;
        case WM_DESTROY:
            PostQuitMessage(EXIT_SUCCESS);
            return 0;
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	void directx::initialize()
	{
        // Create window class
	    w_class = { sizeof(WNDCLASSEX), CS_CLASSDC, wndproc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"app_window", NULL };
		RegisterClassEx(&w_class);

        // Create window
        std::string w_title = "Grand Theft Auto V Cheat Launcher";
        app_window = CreateWindow(w_class.lpszClassName, std::wstring(w_title.begin(), w_title.end()).c_str(), WS_CAPTION | WS_SYSMENU, 100, 100, 500, 500, NULL, NULL, w_class.hInstance, NULL);
	
        // Create DirectX device & swapchain
        if (!create_swapchain())
        {
            // Fatal error -> error handling here
        }

        // Show window
        ShowWindow(app_window, SW_NORMAL);
    }
    void directx::imgui_loop()
    {
        // Load ImGui
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = NULL;
        ImGui_ImplWin32_Init(app_window);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
        std::string FontLocation = filesystem::env_path("SystemDrive") + (std::string)"\\Windows\\Fonts\\Verdana.ttf";
        if (std::filesystem::exists(FontLocation))
            io.Fonts->AddFontFromFileTTF(FontLocation.c_str(), 23.f);

        // Loop
        MSG Message = { 0 };
        while (Message.message != WM_QUIT)
        {
            if (PeekMessage(&Message, NULL, 0U, 0U, PM_REMOVE)) { TranslateMessage(&Message); DispatchMessage(&Message); continue; }

            // Start ImGui Frame
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            // Push Styles
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(ImColor(30, 30, 30, 255)));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 255, 255)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0, 0, 205, 255)));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0, 0, 205, 255)));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(ImColor(50, 50, 50, 255)));
            ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(ImColor(0, 0, 205, 255)));
            ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(ImColor(0, 0, 0, 255)));

            // Make ImGui fill the entire OS window
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
            ImGui::SetNextWindowPos(ImVec2(0, 0));

            // Popup notification
            if (!notification.contents.empty())
            {
                ImGui::OpenPopup(notification.title.empty() ? "###ImGuiNotify" : notification.title.c_str());
                if (ImGui::BeginPopupModal(notification.title.empty() ? "###ImGuiNotify" : notification.title.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
                {
                    ImGui::TextWrapped(notification.contents.c_str());
                    if (ImGui::Button("Close"))
                    {
                        notification.title.clear();
                        notification.contents.clear();
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }

            // Create main ImGui window
            ImGui::Begin("main_imgui", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

            ImGui::Text("Latest cheat version: %s", cheat::github_json["name"].asString());

            if (ImGui::Button("Download & Inject Cheat", ImVec2(ImGui::GetWindowSize().x, 0.f)))
            {
                if (network::download_file_http("https://github.com/HatchesPls/GrandTheftAutoV-Cheat/releases/download/" + cheat::github_json["name"].asString() + "/GTAV.dll", filesystem::paths::CheatModule))
                {

                    module_inject::status status = module_inject::inject(filesystem::paths::CheatModule);
                    if (status == module_inject::status::INJECT_SUCCEEDED)
                    {
                        // Close launcher - injection completed
                        log::MessageBoxWithAutoClose(app_window, "Injection completed!", "Grand Theft Auto V Cheat Launcher", MB_OK | MB_ICONINFORMATION, 10000);
                        Message.message = WM_QUIT;
                    }
                    else if (status == module_inject::status::GAME_NOT_FOUND)
                    {
                        notification.contents = "Please first start the game";
                    }
                    else if (status == module_inject::status::OPENPROCESS_FAILED)
                    {
                        notification.contents = "Injection failed.\n\nTechnical reason: OpenProcess failed";
                    }
                    else if (status == module_inject::status::VIRTUALALLOC_FAILED)
                    {
                        notification.contents = "Injection failed.\n\nTechnical reason: VirtualAlloc failed";
                    }
                    else if (status == module_inject::status::WRITEPROCESSMEM_FAILED)
                    {
                        notification.contents = "Injection failed.\n\nTechnical reason: WriteProcessMem failed";
                    }
                    else if (status == module_inject::status::CREATEREMOTETHREAD_FAILED)
                    {
                        notification.contents = "Injection failed.\n\nTechnical reason: CreateRemoteThread failed";
                    }
                    else if (status == module_inject::status::VIRTUALFREE_FAILED)
                    {
                        notification.contents = "Injection failed.\n\nTechnical reason: VirtualFree failed";
                    }
                    else if (status == module_inject::status::ERROR_EXIT_CODE)
                    {
                        notification.contents = "Injection failed.\n\nTechnical reason: remote thread failure";
                    }
                }
                else
                {
                    notification.contents = "Downloading cheat file failed";
                }
            }

            std::string ChangelogButton = "Show changelog cheat - " + cheat::github_json["name"].asString();
            if (ImGui::Button(ChangelogButton.c_str(), ImVec2(ImGui::GetWindowSize().x, 0.f)))
            {
                notification.title = "Changelog latest release";
                notification.contents = cheat::github_json["body"].asCString();
            }

            ImGui::End();
            ImGui::Render();
            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            // Only render when window is visible
            static UINT presentFlags = 0;
            if (g_pSwapChain->Present(1, presentFlags) == DXGI_STATUS_OCCLUDED)
            {
                presentFlags = DXGI_PRESENT_TEST;
                Sleep(50);
            }
            else
                presentFlags = 0;
        }

        // Cleanup
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        delete_swapchain();
        DestroyWindow(app_window);
        UnregisterClass(w_class.lpszClassName, w_class.hInstance);
    }
}