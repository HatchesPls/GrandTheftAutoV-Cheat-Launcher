#include "filesystem.hpp"

namespace app
{
	const std::string filesystem::paths::UserprofileDir = env_path("userprofile");
	const std::string filesystem::paths::UserDocumentsDir = UserprofileDir + "\\Documents";
	const std::string filesystem::paths::CheatDir = UserDocumentsDir + "\\GTAV Cheat";
	const std::string filesystem::paths::LauncherDir = CheatDir + "\\Launcher";
	const std::string filesystem::paths::CheatModule = LauncherDir + "\\GTAV.dll";

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
}