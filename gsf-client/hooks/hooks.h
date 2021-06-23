#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <hook.h>

namespace hooks
{
	inline WNDPROC o_WindowProc = nullptr;
	LRESULT CALLBACK hk_WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
	
	extern std::unique_ptr<utils::hook_vmt> ch_present;
}