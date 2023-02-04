#include "log.hpp"

namespace app
{
    int log::MessageBoxWithAutoClose(HWND hWnd, const LPCSTR sText, const LPCSTR sCaption, UINT uType, DWORD dwMilliseconds)
    {
        typedef int(__stdcall* MSGBOXWAPI)(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);

        int iResult;
        HMODULE hUser32 = LoadLibraryA("user32.dll");
        if (hUser32)
        {
            auto MessageBoxTimeoutW = (MSGBOXWAPI)GetProcAddress(hUser32, "MessageBoxTimeoutA");
            iResult = MessageBoxTimeoutW(hWnd, sText, sCaption, uType, 0, dwMilliseconds);
            FreeLibrary(hUser32);
        }
        else
            iResult = MessageBoxA(hWnd, sText, sCaption, uType);

        return iResult;
    }
}