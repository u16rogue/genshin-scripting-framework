#include "hooks.h"
#include "../sdk/sdk.h"
#include "../game.h"

game::sdk::Il2CppString *hk_UnityEngine_JsonUtility_ToJson(game::sdk::CS_Object_t *obj, game::sdk::CS_Boolean_t prettyPrint)
{
	static auto o_fn = gsf::hooks::UnityEngine_JsonUtility_ToJson.get_original<decltype(hk_UnityEngine_JsonUtility_ToJson)>();
	auto result = o_fn(obj, prettyPrint);

	return result;
}

utils::hook_detour gsf::hooks::UnityEngine_JsonUtility_ToJson(hk_UnityEngine_JsonUtility_ToJson);