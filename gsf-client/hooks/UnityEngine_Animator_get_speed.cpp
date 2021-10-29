#include "hooks.h"
#include "../game.h"
#include "../callback_manager.h"

void shim_WorldEntityIterator_get_speed(float *rcx_out_float, game::sdk::AnimatorStateInfo_MAYBE *rdx_AnimatorState)
{
	float result = rdx_AnimatorState->speed;
	gsf::callback_manager::get_callbacks().on_animator_get_speed.dispatch_returnable(result, reinterpret_cast<std::uintptr_t>(rdx_AnimatorState->parent), result);
	*rcx_out_float = result;
}

std::uint8_t proxy_WorldEntityIterator_get_speed[] =
{
	0x52,														// push rdx
	0x4C, 0x89, 0xFA,											// mov rdx, r15
	0x48, 0xB8, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,	// mov rax, shim_WorldEntityIterator_get_speed
	0xFF, 0xD0,													// call rax
	0x5A,														// pop rdx
	0xC3														// ret
};

std::uint8_t codeinj_WorldEntityIterator_get_speed[9] =
{
	0xE8, 0x00, 0x00, 0x00, 0x00, // call <proxy_WorldEntityIterator_get_speed - nop_1>
	0x90,						  // nop_1
	0x90,						  // nop_2
	0x90,						  // nop_3
	0x90						  // nop_4
};

float hk_UnityEngine_Animator_get_speed(game::sdk::Animator *animatorObj)
{
	float result = animatorObj->state_info->speed;
	gsf::callback_manager::get_callbacks().on_animator_get_speed.dispatch_returnable(result, reinterpret_cast<std::uintptr_t>(animatorObj), result);
	return result;
}

utils::hook_detour gsf::hooks::UnityEngine_Animator_get_speed(hk_UnityEngine_Animator_get_speed);