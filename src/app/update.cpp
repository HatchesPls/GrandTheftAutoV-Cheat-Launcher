#include "update.hpp"
#include "network.hpp"
#include "filesystem.hpp"
#include "app.hpp"
#include "log.hpp"
#include "version.hpp"
#include <fstream>
#pragma comment(lib, "shell32.lib")

namespace app
{
	void update::check()
	{
		if (launcher::github_json.isMember("name"))
		{
			// If versions do not match, perform auto update (or rollback).

			std::string OnlineVersion = launcher::github_json["name"].asString();
			OnlineVersion.erase(std::remove(OnlineVersion.begin(), OnlineVersion.end(), 'v'), OnlineVersion.end());

			if (OnlineVersion != version_info::version_string)
			{
				std::string launcher_path_and_name = filesystem::launcher_path() + "\\" + filesystem::launcher_filename();

				// Rename current old launcher file
				if (rename(launcher_path_and_name.c_str(), filesystem::paths::OldLauncher.c_str()))
				{
					log::ShowMsgBox("Auto updating launcher failed,  please retrieve it manually from Github.\n\nTechnical details: launcher file rename error", MB_OK | MB_ICONEXCLAMATION, 0);
					std::exit(EXIT_FAILURE);
				}

				// Download new launcher binary
				if (!network::download_file_http("https://github.com/HatchesPls/GrandTheftAutoV-Cheat-Launcher/releases/download/" + launcher::github_json["name"].asString() + "/GTAVCheat_Launcher.exe", launcher_path_and_name))
				{
					log::ShowMsgBox("Downloading new launcher version failed, please retrieve it manually from Github", MB_OK | MB_ICONEXCLAMATION, 0);
					std::exit(EXIT_FAILURE);
				}
	
				// Start new launcher
				ShellExecuteA(NULL, "open", launcher_path_and_name.c_str(), NULL, NULL, SW_SHOWDEFAULT);
				
				// Quite current old launcher
				std::exit(EXIT_SUCCESS);
			}
		}		
	}
}