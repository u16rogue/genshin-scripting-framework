#include "gsf_client.h"
#include "global.h"

#include <Windows.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <misc_utils.h>
#include <console.h>
#include <macro.h>

#include "script_manager.h"
#include "hooks.h"
#include "game.h"

#include <MinHook.h>

#if __has_include("autoexecdef.h") && !defined( GSF_AUTOEXEC_SCRIPT_PATH )
    #include "autoexecdef.h"
#endif

// TODO: better logging by logging to file

static bool init_dx()
{
    DEBUG_COUT("\n[+] Initialize DirectX11..."
               "\n[+] Waiting for the game to create a swapchain and device context...");

    while (!(global::dx_swapchain = *game::dx_swapchain_ptr) && !*game::dx_devicectx_ptr)
        Sleep(800);

    if (!DEBUG_CON_C_LOG(L"Get device", SUCCEEDED(global::dx_swapchain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void **>(&global::dx_device)))))
        return false;

    DEBUG_COUT(" @ 0x" << global::dx_device);

    global::dx_device->GetImmediateContext(&global::dx_context);
    DEBUG_CON_C_LOG(L"Get context", global::dx_context != nullptr);
    DEBUG_COUT(" @ 0x" << global::dx_context);

    ID3D11Texture2D *dx_backbuffer = nullptr;
    if (!DEBUG_CON_C_LOG(L"Get buffer", SUCCEEDED(global::dx_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&dx_backbuffer)))))
        return false;
    DEBUG_COUT(" @ 0x" << dx_backbuffer);

    if (!DEBUG_CON_C_LOG(L"Create render target", dx_backbuffer && SUCCEEDED(global::dx_device->CreateRenderTargetView(dx_backbuffer, nullptr, &global::dx_render_target_view))))
        return false;
    DEBUG_COUT(" @ 0x" << global::dx_render_target_view);

    dx_backbuffer->Release();

    DXGI_SWAP_CHAIN_DESC scd;
    if (!DEBUG_CON_C_LOG(L"Get swapchain description", SUCCEEDED(global::dx_swapchain->GetDesc(&scd))))
        return false;

    if (!DEBUG_CON_C_LOG(L"Initialize ImGui implementation", ImGui_ImplWin32_Init(scd.OutputWindow) && ImGui_ImplDX11_Init(global::dx_device, global::dx_context)))
        return false;

    return true;
}

bool gsf::init()
{
    #ifdef _DEBUG
    {
        if (!con::init() || !SetConsoleTitleW(utils::random_str().c_str()))
            return false;
    }
    #endif

    #ifdef GSF_AUTOEXEC_SCRIPT_PATH
    {
        if (!gsf::script_manager::script_autoexec(GSF_AUTOEXEC_SCRIPT_PATH))
            return false;
    }
    #endif

    if (CURSORINFO ci = { .cbSize = sizeof(ci) }; DEBUG_CON_C_LOG(L"Loading cursor info", GetCursorInfo(&ci)))
        global::cursor_is_visible = ci.flags == CURSOR_SHOWING;

    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;

    if (!game::init() || !init_dx() || !gsf::hooks::install())
        return false;

	return true;
}

// TODO: fix crash when unloading
bool gsf::shutdown()
{
    if (HANDLE exit_thread = nullptr; exit_thread = CreateThread(nullptr, NULL, [](LPVOID arg0) -> DWORD
    {
        gsf::hooks::uninstall();
        gsf::script_manager::unload_all_scripts();

        if (con::is_allocated())
            FreeConsole();

        HWND con_wnd = con::get_window();
        if (con_wnd)
            PostMessageW(con_wnd, WM_CLOSE, NULL, NULL);

        FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(global::dll_handle), 0);
        return 0;
    }, nullptr, NULL, nullptr)) { CloseHandle(exit_thread); }

	return true;
}