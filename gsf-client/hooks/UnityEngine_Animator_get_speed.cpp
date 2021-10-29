#include "hooks.h"
#include "../game.h"
#include "../callback_manager.h"

#include <macro.h>

float hk_UnityEngine_Animator_get_speed(game::sdk::Animator *animatorObj)
{
	float result = animatorObj->unk0->speed;
	gsf::callback_manager::get_callbacks().on_animator_get_speed.dispatch_returnable(result, reinterpret_cast<std::uintptr_t>(animatorObj), result);

	return result;
}

utils::hook_detour gsf::hooks::UnityEngine_Animator_get_speed(hk_UnityEngine_Animator_get_speed);