#include <Windows.h>
#include "global.h"
#include "gsf_client.h"
#include "hooks/hooks.h"
#include <chaiscript.hpp>

// TODO: add hooking helpers, clean up dllmain and separate everything neatly

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (HANDLE entry_thread = nullptr; ul_reason_for_call == DLL_PROCESS_ATTACH && (entry_thread = CreateThread(nullptr, NULL, [](LPVOID hModule) -> DWORD
    {
        global::dll_handle = hModule;
        const HMODULE &hmod = reinterpret_cast<HMODULE>(hModule);

        DisableThreadLibraryCalls(hmod);
        
        if (!gsf::load())
        {
            FreeLibraryAndExitThread(hmod, 0);
            return 0;
            //TerminateProcess(GetCurrentProcess(), 0); // :)
        }

        return 0;
    }, hModule, NULL, nullptr))) { CloseHandle(entry_thread); }

    return TRUE;
}

