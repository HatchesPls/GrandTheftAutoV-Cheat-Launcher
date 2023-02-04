#include "entry_point.hpp"
#include "../directx/directx.hpp"
#include "network.hpp"
#include "../cheat/cheat.hpp"
#include "filesystem.hpp"

using namespace app;

// App entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ int nShowCmd)
{
	// Setup directory
	if (!std::filesystem::exists(filesystem::paths::DataDir))
	{
		std::filesystem::create_directory(filesystem::paths::DataDir);

		// Set "Launcher" directory hidden - user rarerly needs to access it directly
		SetFileAttributesA(filesystem::paths::DataDir.c_str(), FILE_ATTRIBUTE_HIDDEN);
	}

	// Launcher update check with auto update - TODO
		
	// Make sure Github API rate limit is not reached for our current public IP address
	if (network::github_api_rate_limit_reached())
	{
		MessageBoxA(NULL, "Failed to fetch data from Github, API rate limit has been reached. Please wait 30-60 minutes and try again.", "Grand Theft Auto V Cheat Launcher", MB_OK | MB_ICONEXCLAMATION);
		std::exit(EXIT_FAILURE);
	}

	// Fetch cheat data from Github
	Json::Value gh_cdata;
	if (network::get_json_object_url("https://api.github.com/repos/HatchesPls/GrandTheftAutoV-Cheat/releases/latest", gh_cdata))
	{
		cheat::latest_version = gh_cdata["name"].asString();
	}
	else
	{
		MessageBoxA(NULL, "Failed to fetch data from Github, cannot procceed.\n\n1. Verify you have internet connectivity\n\n2. Can you access api.github.com through a webbrowser?", "Grand Theft Auto V Cheat Launcher", MB_OK | MB_ICONEXCLAMATION);
		std::exit(EXIT_FAILURE);
	}

	// Init directX
	directx::initialize();

	// Start ImGui Loop
	directx::imgui_loop();

	// Loop ended - exit
	return EXIT_SUCCESS;
}