#include "hooks.h"

#include <Windows.h>
#include <d3d11.h>
#include <imgui.h>

#include <imgui_impl_win32.h>
#include <imgui_internal.h>

#include "../gsf_client.h"

#include "../global.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK hk_WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (uMsg == WM_KEYDOWN && wParam == VK_DELETE)
        gsf::shutdown();

    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
        return TRUE;

    static auto &imgui_io = ImGui::GetIO();
    if (imgui_io.WantCaptureKeyboard || imgui_io.WantCaptureMouse)
        return TRUE;
    
    static auto o_WindowProc = hooks::ch_wndproc->get_original<decltype(hk_WindowProc)>();
    return o_WindowProc(hwnd, uMsg, wParam, lParam);
}

std::unique_ptr<utils::hook_wndproc> hooks::ch_wndproc = std::make_unique<utils::hook_wndproc>(hk_WindowProc);