#pragma once
#include <string>
#include <json/json.h>

namespace app
{
	class network
	{
	public:
		static bool is_reachable(std::string url);
		static bool get_json_object_url(std::string url, Json::Value& jsoncpp_object);
		static bool github_api_rate_limit_reached();
		static bool download_file_http(std::string url, std::string path);
	};
}