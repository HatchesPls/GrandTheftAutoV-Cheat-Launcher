#pragma once
#include <Windows.h>

namespace app
{
	class log
	{
	public:
        // https://stackoverflow.com/questions/3091300/messagebox-with-timeout-or-closing-a-messagebox-from-another-thread
		static int MessageBoxWithAutoClose(HWND hWnd, const LPCSTR sText, const LPCSTR sCaption, UINT uType, DWORD dwMilliseconds = 0);
	};
}