#pragma once
#include <Windows.h>
#include <string>

namespace app
{
	class log
	{
	private:
		// https://stackoverflow.com/questions/3091300/messagebox-with-timeout-or-closing-a-messagebox-from-another-thread
		static int MessageBoxWithAutoClose(HWND hWnd, const LPCSTR sText, const LPCSTR sCaption, UINT uType, DWORD dwMilliseconds = 0);
	public:
		static int ShowMsgBox(std::string text, UINT type_flags, DWORD close_delay = 0);
	};
}