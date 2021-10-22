#include "hooks.h"
#include "../game.h"

float hk_UnityEngine_Animator_get_speed(game::sdk::Animator *animatorObj)
{
	static auto o_fn = gsf::hooks::UnityEngine_Animator_get_speed.get_original<decltype(hk_UnityEngine_Animator_get_speed)>();
	float result = o_fn(animatorObj);
	return result;
}

utils::hook_detour gsf::hooks::UnityEngine_Animator_get_speed(hk_UnityEngine_Animator_get_speed);