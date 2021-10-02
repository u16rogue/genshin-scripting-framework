#include "hooks.h"
#include "../sdk/sdk.h"
#include "../game.h"
#include "../api/api_controls.h"
#include <hash.h>

#include <macro.h>

// MouseButton0 - l
// MouseButton2 - m
// MouseButton1 - r
// MouseButton3 - m4
// MouseButton4 - m5


game::sdk::CS_Boolean_t hk_UnityEngine_Input_GetButton(game::sdk::Il2CppString *buttonName)
{
	switch (utils::hash_fnv1a(game::il2cpp_string_chars(buttonName)))
	{
		case utils::hash_fnv1a(L"MouseButton0"):
		{
			if (gsf::api_controls::flag_mouse_left_down)
			{
				if (gsf::api_controls::flag_mouse_left_down > 0)
					--gsf::api_controls::flag_mouse_left_down;

				return true;
			}
			break;
		}

		case utils::hash_fnv1a(L"MouseButton1"):
		{
			if (gsf::api_controls::flag_mouse_right_down)
			{
				if (gsf::api_controls::flag_mouse_right_down > 0)
					--gsf::api_controls::flag_mouse_right_down;

				return true;
			}
			break;
		}

		case utils::hash_fnv1a(L"MouseButton2"):
		{
			if (gsf::api_controls::flag_mouse_middle_down)
			{
				if (gsf::api_controls::flag_mouse_middle_down > 0)
					--gsf::api_controls::flag_mouse_middle_down;

				return true;
			}
			break;
		}
	}

	static auto o_fn = gsf::hooks::UnityEngine_Input_GetButton.get_original<decltype(hk_UnityEngine_Input_GetButton)>();
	return o_fn(buttonName);
}

utils::hook_detour gsf::hooks::UnityEngine_Input_GetButton(hk_UnityEngine_Input_GetButton);