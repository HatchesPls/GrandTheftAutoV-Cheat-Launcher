#pragma once
#include <Windows.h>
#include <string>

namespace app
{
	class module_inject
	{
	private:
		static bool get_process_id_by_name(std::string p_name, DWORD& p_id);
	public:
		enum class status
		{
			GAME_NOT_FOUND,
			DOWNLOAD_FAILED,
			OPENPROCESS_FAILED,
			VIRTUALALLOC_FAILED,
			WRITEPROCESSMEM_FAILED,
			CREATEREMOTETHREAD_FAILED,
			VIRTUALFREE_FAILED,
			ERROR_EXIT_CODE,
			INJECT_SUCCEEDED
		};
		static status inject();
	};
}