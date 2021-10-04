#include "hooks.h"
#include "../sdk/sdk.h"
#include "../api/api_controls.h"
#include "../game.h"
#include <hash.h>

// MouseAxis2
// +y
// --
// -y

// MouseAxis1
// -x | +x

float hk_UnityEngine_Input_GetAxisRaw(game::sdk::Il2CppString *axisName)
{
	auto hashed_axis_name = utils::hash_fnv1a(game::il2cpp_string_chars(axisName));
	for (auto &force_axis : gsf::api_controls::flags_look)
	{
		if (force_axis.game_identifier != hashed_axis_name)
			continue;

		if (force_axis.amount == 0.f)
			break;

		float force_val = force_axis.amount;
		force_axis.amount = 0.f;

		return force_val;
	}

	static auto o_fn = gsf::hooks::UnityEngine_Input_GetAxisRaw.get_original<decltype(hk_UnityEngine_Input_GetAxisRaw)>();
	return o_fn(axisName);
}

utils::hook_detour gsf::hooks::UnityEngine_Input_GetAxisRaw(hk_UnityEngine_Input_GetAxisRaw);
