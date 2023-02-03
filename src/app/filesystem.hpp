#pragma once
#include <string>
#include <filesystem>

namespace app
{
	class filesystem
	{
	public:
		static std::string env_path(std::string var);
	};
}