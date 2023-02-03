#include "filesystem.hpp"

namespace app
{
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