#include "entry_point.hpp"
#include "../directx/directx.hpp"

using namespace app;

// App entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ int nShowCmd)
{
	// Init directX
	directx::initialize();

	// Start ImGui Loop
	directx::imgui_loop();
}