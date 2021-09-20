#include "hooks.h"
#include "../sdk/sdk.h"
#include "../menu/menu.h"

void hk_UnityEngine_Cursor_set_lockState(game::sdk::CursorLockMode mode)
{
	if (gsf::menu::is_open && mode == game::sdk::CursorLockMode::Locked)
		return;

	static auto o_fn = gsf::hooks::UnityEngine_Cursor_set_lockState.get_original<decltype(hk_UnityEngine_Cursor_set_lockState)>();
	return o_fn(mode);
}

utils::hook_detour gsf::hooks::UnityEngine_Cursor_set_lockState(hk_UnityEngine_Cursor_set_lockState);