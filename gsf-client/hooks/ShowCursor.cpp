#include "hooks.h"
#include <Windows.h>
#include "../global.h"

int __stdcall hk_ShowCursor(BOOL bShow)
{
	static auto o_ShowCursor = gsf::hooks::ch_showcursor->get_original<decltype(hk_ShowCursor)>();
	auto result = o_ShowCursor(bShow);
	global::cursor_is_visible = result != -1;

	return result;
}

std::unique_ptr<utils::hook_detour> gsf::hooks::ch_showcursor = std::make_unique<utils::hook_detour>(hk_ShowCursor);