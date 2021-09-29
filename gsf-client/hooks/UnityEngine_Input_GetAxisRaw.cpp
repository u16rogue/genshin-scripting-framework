#include "hooks.h"
#include "../sdk/sdk.h"

float hk_UnityEngine_Input_GetAxisRaw(game::sdk::Il2CppString *axisName)
{
	static auto o_fn = gsf::hooks::UnityEngine_Input_GetAxisRaw.get_original<decltype(hk_UnityEngine_Input_GetAxisRaw)>();
	auto result = o_fn(axisName);
	return result;
}

utils::hook_detour gsf::hooks::UnityEngine_Input_GetAxisRaw(hk_UnityEngine_Input_GetAxisRaw);
