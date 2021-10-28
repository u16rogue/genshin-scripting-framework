#include "hooks.h"
#include "../game.h"
#include "../callback_manager.h"

float hk_UnityEngine_Animator_get_speed(game::sdk::Animator *animatorObj)
{
	static auto o_fn = gsf::hooks::UnityEngine_Animator_get_speed.get_original<decltype(hk_UnityEngine_Animator_get_speed)>();
	float result = o_fn(animatorObj);
	gsf::callback_manager::get_callbacks().on_animator_get_speed.dispatch_returnable(result, reinterpret_cast<std::uintptr_t>(animatorObj), result);

	return result;
}

utils::hook_detour gsf::hooks::UnityEngine_Animator_get_speed(hk_UnityEngine_Animator_get_speed);