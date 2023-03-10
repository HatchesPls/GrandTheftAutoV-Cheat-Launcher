#include "module_inject.hpp"
#include "../cheat/cheat.hpp"
#include "../app/network.hpp"
#include "../app/filesystem.hpp"
#include <Tlhelp32.h>
#include <array>

namespace app
{
	bool module_inject::get_process_id_by_name(std::string p_name, DWORD& p_id)
	{
        std::wstring p_name_ws(p_name.begin(), p_name.end());

        bool target_found = false;

        PROCESSENTRY32 processInfo;
        processInfo.dwSize = sizeof(processInfo);

        HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        if (processesSnapshot == INVALID_HANDLE_VALUE)
            return false;

        Process32First(processesSnapshot, &processInfo);
        if (!p_name_ws.compare(processInfo.szExeFile))
        {
            p_id = processInfo.th32ProcessID;
            target_found = true;
        }

        while (Process32Next(processesSnapshot, &processInfo))
        {
            if (!p_name_ws.compare(processInfo.szExeFile))
            {
                p_id = processInfo.th32ProcessID;
                target_found = true;
            }
        }

        CloseHandle(processesSnapshot);

        return target_found;
	}
    module_inject::status module_inject::inject()
	{
        // Retrieve GTA5.exe process ID
        DWORD gta5_process_id;
        if (!get_process_id_by_name("GTA5.exe", gta5_process_id))
            return status::GAME_NOT_FOUND;

        // Download cheat DLL from Github
        if (!network::download_file_http("https://github.com/HatchesPls/GrandTheftAutoV-Cheat/releases/download/" + cheat::github_json["name"].asString() + "/GTAV.dll", filesystem::paths::CheatModule))
            return status::DOWNLOAD_FAILED;

        // Open game process handle
        HANDLE gta5_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, gta5_process_id);
        if (!gta5_process)
            return status::OPENPROCESS_FAILED; // Possible cause: GTA5.exe is running elevated which means the launcher also needs to be ran elevated

        // Allocate memory region in GTA5.exe
        LPVOID Memory = LPVOID(VirtualAllocEx(gta5_process, NULL, std::size(filesystem::paths::CheatModule), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
        if (!Memory)
            return status::VIRTUALALLOC_FAILED;

        // Write cheat data to the created memory region in GTA5.exe
        if (!WriteProcessMemory(gta5_process, Memory, filesystem::paths::CheatModule.c_str(), size(filesystem::paths::CheatModule), NULL))
            return status::WRITEPROCESSMEM_FAILED;

        // Call DLL_Main function of cheat inside GTA5.exe
        HANDLE RemoteThreadHandle = CreateRemoteThread(gta5_process, NULL, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), Memory, NULL, NULL);
        if (!RemoteThreadHandle)
            return status::CREATEREMOTETHREAD_FAILED;

        // Wait for remote thread start
        WaitForSingleObject(RemoteThreadHandle, INFINITE);

        // Get remote thread exit code (the return code with which dll_main exited)
        DWORD RThreadExitCode = 0;
        GetExitCodeThread(RemoteThreadHandle, &RThreadExitCode);
        CloseHandle(RemoteThreadHandle);

        // Cleanup allocated GTA5.exe memory
        if (!VirtualFreeEx(gta5_process, Memory, NULL, MEM_RELEASE))
            return status::VIRTUALFREE_FAILED;

        // Close GTA5.exe process handle
        CloseHandle(gta5_process);

        // Did injection succeed?
        if (!RThreadExitCode)
            return status::ERROR_EXIT_CODE;

        return status::INJECT_SUCCEEDED;
	}
}