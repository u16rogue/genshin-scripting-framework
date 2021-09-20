#pragma once

#include <hooking.h>

namespace gsf::hooks
{
	extern utils::hook_detour  Present;
	extern utils::hook_wndproc WindowProc;
	extern utils::hook_detour  Draw;
	extern utils::hook_detour  DrawIndexed;
	extern utils::hook_detour  ShowCursor;
	extern utils::hook_detour  UnityEngine_Cursor_set_lockState;
	extern utils::hook_detour  UnityEngine_Cursor_set_visible;

	bool install();
	bool uninstall();
}