#include "entry_point.hpp"
#include "app.hpp"
#include "update.hpp"
#include "../directx/directx.hpp"
#include "log.hpp"
#include "network.hpp"
#include "../cheat/cheat.hpp"
#include "filesystem.hpp"

using namespace app;

// App entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ int nShowCmd)
{
	// We only want one launcher instance running
	if (!std::filesystem::exists(filesystem::paths::OldLauncher))
	{
		auto instance_h = FindWindowA("gtav_cl_app", NULL);
		if (instance_h)
		{
			// Another instance is already running - set that instance's window to foreground
			SetForegroundWindow(instance_h);
			CloseHandle(instance_h);
			std::exit(EXIT_SUCCESS);
		}
	}
	else
	{
		// Auto update completed. Delete old launcher file
		std::remove(filesystem::paths::OldLauncher.c_str());
	}

	// Create directories
	if (!std::filesystem::exists(filesystem::paths::CheatDir))
		std::filesystem::create_directory(filesystem::paths::CheatDir);
	if (!std::filesystem::exists(filesystem::paths::LauncherDir))
		std::filesystem::create_directory(filesystem::paths::LauncherDir);

	// Make sure Github API rate limit is not reached for our current public IP address
	if (network::github_api_rate_limit_reached())
	{
		log::ShowMsgBox("Failed to fetch data from Github because API rate limit has been reached.\n\nPlease wait 30-60 minutes and try again.", MB_OK | MB_ICONEXCLAMATION);
		std::exit(EXIT_FAILURE);
	}

	// Fetch launcher data from Github
	if (!network::get_json_object_url("https://api.github.com/repos/HatchesPls/GrandTheftAutoV-Cheat-Launcher/releases/latest", launcher::github_json))
	{
		log::ShowMsgBox("Failed to fetch launcher data from Github.\n\n1. Verify you have internet connectivity\n\n2. Can you access api.github.com through a webbrowser?", MB_OK | MB_ICONEXCLAMATION);
		std::exit(EXIT_FAILURE);
	}

	// Do launcher update
	update::check();

	// Fetch cheat data from Github
	if (!network::get_json_object_url("https://api.github.com/repos/HatchesPls/GrandTheftAutoV-Cheat/releases/latest", cheat::github_json))
	{
		log::ShowMsgBox("Failed to fetch cheat data from Github.\n\n1. Verify you have internet connectivity\n\n2. Can you access api.github.com through a webbrowser?", MB_OK | MB_ICONEXCLAMATION);
		std::exit(EXIT_FAILURE);
	}

	// Init directX
	directx::initialize();

	// Start ImGui Loop
	directx::imgui_loop();

	// Loop ended - exit
	return EXIT_SUCCESS;
}