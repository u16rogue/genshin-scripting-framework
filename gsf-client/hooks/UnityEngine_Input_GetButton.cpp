#include "hooks.h"
#include "../sdk/sdk.h"

game::sdk::CS_Boolean_t hk_UnityEngine_Input_GetButton(game::sdk::Il2CppString *buttonName)
{
	static auto o_fn = gsf::hooks::UnityEngine_Input_GetButton.get_original<decltype(hk_UnityEngine_Input_GetButton)>();
	auto result = o_fn(buttonName);
	return result;
}

utils::hook_detour gsf::hooks::UnityEngine_Input_GetButton(hk_UnityEngine_Input_GetButton);