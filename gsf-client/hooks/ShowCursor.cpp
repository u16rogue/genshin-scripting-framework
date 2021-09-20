#include "hooks.h"
#include <Windows.h>
#include "../global.h"

int __stdcall hk_ShowCursor(BOOL bShow)
{
	static auto o_ShowCursor = gsf::hooks::WIN_ShowCursor.get_original<decltype(hk_ShowCursor)>();
	auto result = o_ShowCursor(bShow);
	global::cursor_is_visible = result != -1;

	return result;
}

utils::hook_detour gsf::hooks::WIN_ShowCursor(hk_ShowCursor);