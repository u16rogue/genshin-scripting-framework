#include "hooks.h"
#include "../sdk/sdk.h"
#include "../game.h"
#include "../api/api_controls.h"
#include <hash.h>

// MouseButton0 - l
// MouseButton2 - m
// MouseButton1 - r
// MouseButton3 - m4
// MouseButton4 - m5

game::sdk::Boolean hk_UnityEngine_Input_GetButton(game::sdk::Il2CppString *buttonName)
{
	auto hashed_btn_name = utils::hash_fnv1a(game::il2cpp_string_chars(buttonName));
	for (auto &force_key : gsf::api_controls::flags_mouse)
	{
		if (force_key.game_identifier != hashed_btn_name)
			continue;

		if (force_key.ticks == 0)
			break;

		if (force_key.ticks > 0)
			--force_key.ticks;

		return true;
	}

	static auto o_fn = gsf::hooks::UnityEngine_Input_GetButton.get_original<decltype(hk_UnityEngine_Input_GetButton)>();
	return o_fn(buttonName);
}

utils::hook_detour gsf::hooks::UnityEngine_Input_GetButton(hk_UnityEngine_Input_GetButton);