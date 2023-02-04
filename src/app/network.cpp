#include "network.hpp"
#include <Windows.h>
#include <WinINet.h>
#pragma comment(lib, "WinINet.lib")
#pragma comment(lib, "Urlmon.lib")
#include <json\json.h>

namespace app
{
	bool network::is_reachable(std::string url)
	{
		if (InternetCheckConnectionA(url.c_str(), FLAG_ICC_FORCE_CONNECTION, NULL))
			return true;
		return false;
	}
	bool network::download_file_http(std::string url, std::string path)
	{
		// No need to remove target file - URLDownloadToFile automatically overwrites it

		// Make sure the download location is reachable
		if (!is_reachable(url))
			return false;

		// Start and wait for download to complete - Did it succeed?
		if (URLDownloadToFileA(NULL, url.c_str(), path.c_str(), 0, NULL) == S_OK)
			return true;

		return false;
	}
	// TODO: the API also provides the UNIX time after which the limit is reset, might want to report that back to the user?
	bool network::github_api_rate_limit_reached()
	{
		Json::Value json;
		if (get_json_object_url("https://api.github.com/rate_limit", json))
		{
			if (json["resources"]["core"]["remaining"].asInt() == 0)
				return true;
		}	
		return false;
	}
	bool network::get_json_object_url(std::string url, Json::Value& jsoncpp_object)
	{
		bool return_v = false;

		// Make sure target url is reachable
		if (!is_reachable(url))
			return return_v;

		HINTERNET hopen = InternetOpen(L"gtav_launcher", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
		if (hopen)
		{
			HINTERNET hinternet = InternetOpenUrlA(hopen, url.c_str(), NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE, NULL);
			if (hinternet)
			{
				char tmp[2048 + 1];
				DWORD received = 0;
				std::string buffer;
				while (InternetReadFile(hinternet, tmp, 2048, &received) && received > 0)
				{
					if (!received) break;
					tmp[received] = '\0';
					buffer += (std::string)tmp;
				}

				Json::CharReaderBuilder CharBuilder;		
				JSONCPP_STRING JsonError;
				const std::unique_ptr<Json::CharReader> reader(CharBuilder.newCharReader());
				if (reader->parse(buffer.c_str(), buffer.c_str() + buffer.length(), &jsoncpp_object, &JsonError))
					return_v = true; // JsonCpp succesfully parsed downloaded JSON data

				InternetCloseHandle(hinternet);
			}
			InternetCloseHandle(hopen);
		}
		return return_v;
	}
}