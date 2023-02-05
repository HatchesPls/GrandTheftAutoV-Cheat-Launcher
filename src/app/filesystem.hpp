#pragma once
#include <string>
#include <filesystem>

namespace app
{
	class filesystem
	{
	public:
		class paths
		{
		public:
			static const std::string UserprofileDir;
			static const std::string UserDocumentsDir;
			static const std::string CheatDir;
			static const std::string LauncherDir;
			static const std::string CheatModule;
			static const std::string OldLauncher;
		};
		static std::string env_path(std::string var);
		static std::string launcher_filename();
		static std::string launcher_path();
	};
}