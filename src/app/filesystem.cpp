#include "filesystem.hpp"
#include <Windows.h>

namespace app
{
	const std::string filesystem::paths::UserprofileDir = env_path("userprofile");
	const std::string filesystem::paths::UserDocumentsDir = UserprofileDir + "\\Documents";
	const std::string filesystem::paths::CheatDir = UserDocumentsDir + "\\GTAV Cheat";
	const std::string filesystem::paths::LauncherDir = CheatDir + "\\Launcher";
	const std::string filesystem::paths::CheatModule = LauncherDir + "\\GTAV.dll";
	const std::string filesystem::paths::OldLauncher = launcher_path() + "\\GTAVCheat_Launcher.exe.old";

	std::string filesystem::env_path(std::string var)
	{
		char* buf = nullptr;
		std::string ReturnVal;
		size_t sz = 0;
		if (_dupenv_s(&buf, &sz, var.c_str()) == 0 && buf != nullptr)
		{
			ReturnVal = buf;
			free(buf);
		}
		return ReturnVal;
	}
	std::string filesystem::launcher_filename()
	{
		char launcher_filename[MAX_PATH];
		GetModuleFileNameA(NULL, launcher_filename, MAX_PATH);
		return std::filesystem::path(launcher_filename).filename().string();
	}
	std::string filesystem::launcher_path()
	{
		char launcher_file_path[MAX_PATH];
		GetModuleFileNameA(NULL, launcher_file_path, MAX_PATH);
		return std::filesystem::path(launcher_file_path).parent_path().string();
	}
}