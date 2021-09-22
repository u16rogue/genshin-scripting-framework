#include "hooks.h"
#include <Windows.h>
#include "../global.h"
#include "../menu/menu.h"

int __stdcall hk_ShowCursor(BOOL bShow)
{
	if (gsf::menu::is_open && !bShow)
		return 0;

	static auto o_ShowCursor = gsf::hooks::ShowCursor.get_original<decltype(hk_ShowCursor)>();
	auto result = o_ShowCursor(bShow);
	global::cursor_is_visible = result != -1;

	return result;
}

utils::hook_detour gsf::hooks::ShowCursor(hk_ShowCursor);