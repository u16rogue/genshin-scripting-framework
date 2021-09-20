#include "hooks.h"
#include "../menu/menu.h"

void hk_UnityEngine_Cursor_set_visible(int visible)
{
	if (gsf::menu::is_open && !visible)
		return;

	static auto o_fn = gsf::hooks::UnityEngine_Cursor_set_visible.get_original<decltype(hk_UnityEngine_Cursor_set_visible)>();
	return o_fn(visible);
}

utils::hook_detour gsf::hooks::UnityEngine_Cursor_set_visible(hk_UnityEngine_Cursor_set_visible);