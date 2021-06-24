#include <Windows.h>
#include "global.h"
#include <console.h>
#include <misc_utils.h>
#include "hooks/hooks.h"

// TODO: add hooking helpers, clean up dllmain and separate everything neatly

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (HANDLE entry_thread = nullptr; ul_reason_for_call == DLL_PROCESS_ATTACH && (entry_thread = CreateThread(nullptr, NULL, [](LPVOID hModule) -> DWORD
    {
        global::dll_handle = hModule;
        const HMODULE &hmod = reinterpret_cast<HMODULE>(hModule);

        DisableThreadLibraryCalls(hmod);
        
        if ((global::game_window = FindWindowW(L"UnityWndClass", L"Genshin Impact")) == nullptr
        #ifdef _DEBUG
        || !con::init()
        || !SetConsoleTitleW(utils::random_str().c_str())
        #endif
        || !hooks::install()
        ) {
            FreeLibraryAndExitThread(hmod, 0);
            return 0;
            //TerminateProcess(GetCurrentProcess(), 0); // :)
        }

        return 0;
    }, hModule, NULL, nullptr))) { CloseHandle(entry_thread); }

    return TRUE;
}

