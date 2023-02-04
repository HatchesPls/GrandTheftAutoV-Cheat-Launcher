#include "entry_point.hpp"
#include "../directx/directx.hpp"
#include "network.hpp"
#include "../cheat/cheat.hpp"
#include "filesystem.hpp"

using namespace app;

// App entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ int nShowCmd)
{
	// Create directories
	if (!std::filesystem::exists(filesystem::paths::CheatDir))
		std::filesystem::create_directory(filesystem::paths::CheatDir);
	if (!std::filesystem::exists(filesystem::paths::LauncherDir))
		std::filesystem::create_directory(filesystem::paths::LauncherDir);

	// Launcher update check with auto update - TODO
		
	// Make sure Github API rate limit is not reached for our current public IP address
	if (network::github_api_rate_limit_reached())
	{
		MessageBoxA(NULL, "Failed to fetch data from Github, API rate limit has been reached.\n\nPlease wait 30-60 minutes and try again.", "Grand Theft Auto V Cheat Launcher", MB_OK | MB_ICONEXCLAMATION);
		std::exit(EXIT_FAILURE);
	}

	// Fetch cheat data from Github
	if (!network::get_json_object_url("https://api.github.com/repos/HatchesPls/GrandTheftAutoV-Cheat/releases/latest", cheat::github_json))
	{
		MessageBoxA(NULL, "Failed to fetch data from Github.\n\n1. Verify you have internet connectivity\n\n2. Can you access api.github.com through a webbrowser?", "Grand Theft Auto V Cheat Launcher", MB_OK | MB_ICONEXCLAMATION);
		std::exit(EXIT_FAILURE);
	}

	// Init directX
	directx::initialize();

	// Start ImGui Loop
	directx::imgui_loop();

	// Loop ended - exit
	return EXIT_SUCCESS;
}