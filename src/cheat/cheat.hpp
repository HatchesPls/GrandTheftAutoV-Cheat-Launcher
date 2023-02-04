#pragma once
#include <string>
#include "json/json.h"

namespace app
{
	class cheat
	{
	public:
		static Json::Value github_json;		// Contains all Github API data for the latest cheat release
	};
}