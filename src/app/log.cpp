#include "log.hpp"
#include "../directx/directx.hpp"

namespace app
{
    int log::MessageBoxWithAutoClose(HWND hWnd, const LPCSTR sText, const LPCSTR sCaption, UINT uType, DWORD dwMilliseconds)
    {
        int result;
        typedef int(__stdcall* MSGBOXWAPI)(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);

        HMODULE user32 = LoadLibraryA("user32.dll");
        if (user32 && dwMilliseconds > 0)
        { 
            auto MessageBoxTimeoutA = (MSGBOXWAPI)GetProcAddress(user32, "MessageBoxTimeoutA");
            result = MessageBoxTimeoutA(hWnd, sText, sCaption, uType, 0, dwMilliseconds);      
            FreeLibrary(user32);
        }
        else
        {
            result = MessageBoxA(hWnd, sText, sCaption, uType);
        }

        return result;
    }
    int log::ShowMsgBox(std::string text, UINT type_flags, DWORD close_delay)
    {
        return MessageBoxWithAutoClose(directx::app_window, text.c_str(), "Grand Theft Auto V Cheat Launcher", type_flags, close_delay);
    }
}