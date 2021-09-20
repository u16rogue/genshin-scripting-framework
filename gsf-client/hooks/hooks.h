#pragma once

#include <hooking.h>

namespace gsf::hooks
{
	extern utils::hook_detour  DX_Present;
	extern utils::hook_wndproc WndProc;
	extern utils::hook_detour  DX_Draw;
	extern utils::hook_detour  DX_DrawIndexed;
	extern utils::hook_detour  WIN_ShowCursor;


	bool install();
	bool uninstall();
}