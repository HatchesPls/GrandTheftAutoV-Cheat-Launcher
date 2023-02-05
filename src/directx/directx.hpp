#pragma once
#include <Windows.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <string>

namespace app
{
	class directx
	{
	private:
		static WNDCLASSEX w_class;
		static ID3D11Device* g_pd3dDevice;
		static IDXGISwapChain* g_pSwapChain;
		static ID3D11DeviceContext* g_pd3dDeviceContext;
		static ID3D11RenderTargetView* g_mainRenderTargetView;

		static void cleanup_render_target();
		static void create_render_target();
		static bool create_swapchain();
		static void delete_swapchain();
		static LRESULT WINAPI wndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	public:
		static HWND app_window;

		struct imgui_notify
		{
			std::string title;
			std::string contents;
		};

		static void initialize();
		static void imgui_loop();
	};
}