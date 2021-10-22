#include "hooks.h"
#include "../sdk/sdk.h"

game::sdk::Il2CppString *hk_UnityEngine_JsonUtility_ToJson(game::sdk::Object *obj, game::sdk::Boolean prettyPrint)
{
	static auto o_fn = gsf::hooks::UnityEngine_JsonUtility_ToJson.get_original<decltype(hk_UnityEngine_JsonUtility_ToJson)>();
	auto result = o_fn(obj, prettyPrint);

	return result;
}

utils::hook_detour gsf::hooks::UnityEngine_JsonUtility_ToJson(hk_UnityEngine_JsonUtility_ToJson);