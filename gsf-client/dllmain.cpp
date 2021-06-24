#include <Windows.h>
#include "global.h"
#include <console.h>
#include <misc_utils.h>
#include <hooking.h>
#include <macro.h>
#include "helpers/dx11_dummy.h"
#include "hooks/hooks.h"
#include <MinHook.h>

// TODO: add hooking helpers, clean up dllmain and separate everything neatly

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (HANDLE entry_thread = nullptr; ul_reason_for_call == DLL_PROCESS_ATTACH && (entry_thread = CreateThread(nullptr, NULL, [](LPVOID hModule) -> DWORD
    {
        global::dll_handle = hModule;
        const HMODULE &hmod = reinterpret_cast<HMODULE>(hModule);

        DisableThreadLibraryCalls(hmod);

        if (!con::init())
        {
            FreeLibraryAndExitThread(hmod, 0);
            return 0;
        }

        SetConsoleTitleW(utils::random_str().c_str());
        
        if (MH_Initialize() != MH_OK)
            return 0;

        while ( (global::game_window = FindWindowW(L"UnityWndClass", L"Genshin Impact")) == nullptr)
            Sleep(1000);
        DEBUG_WCOUT("\nGame window: 0x" << global::game_window);

        // Hook window proc
        hooks::ch_wndproc->init(global::game_window);
        hooks::ch_wndproc->hook();

        // Hook IDXGISwapChain::Present
        ID3D11Device   *dummy_device_ptr;
        IDXGISwapChain *dummy_swapchain_ptr;
        if (!helpers::dx11_dummy_device_dup(reinterpret_cast<HWND>(global::game_window), dummy_device_ptr, dummy_swapchain_ptr))
            return 0;
        
        hooks::ch_present->init(reinterpret_cast<void ***>(dummy_swapchain_ptr)[0][8]);
        hooks::ch_present->hook();

        dummy_swapchain_ptr->Release();
        dummy_device_ptr->Release();

        return 0;
    }, hModule, NULL, nullptr))) { CloseHandle(entry_thread); }

    return TRUE;
}

