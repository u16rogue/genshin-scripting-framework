#pragma once

#include <hooking.h>

namespace gsf::hooks
{
	extern utils::hook_detour  Present;
	extern utils::hook_wndproc WindowProc;
	extern utils::hook_detour  Draw;
	extern utils::hook_detour  DrawIndexed;
	extern utils::hook_detour  ShowCursor;

	bool install();
	bool uninstall();
}