#include "hooks.h"
#include <Windows.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include "../gsf_client.h"
#include "../menu/menu.h"
#include "../script_manager.h"
#include "../callback_manager.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK hk_WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (uMsg == WM_KEYDOWN && wParam == VK_DELETE)
        gsf::shutdown();

    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
        return TRUE;

    if ((uMsg == WM_KEYDOWN || uMsg == WM_KEYUP) && gsf::callback_manager::get_callbacks().on_key.dispatch_cancellable(uMsg == WM_KEYDOWN, wParam))
        return TRUE;

    if (gsf::menu::windowproc(uMsg, wParam, lParam))
        return TRUE;

    if (static auto &imgui_io = ImGui::GetIO(); imgui_io.WantCaptureKeyboard || imgui_io.WantCaptureMouse)
        return TRUE;

    static auto o_WindowProc = gsf::hooks::WindowProc.get_original<decltype(hk_WindowProc)>();
    return o_WindowProc(hwnd, uMsg, wParam, lParam);
}

utils::hook_wndproc gsf::hooks::WindowProc(hk_WindowProc);