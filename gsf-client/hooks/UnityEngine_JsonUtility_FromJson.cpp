#include "hooks.h"
#include "../sdk/sdk.h"

game::sdk::CS_Object_t *hk_UnityEngine_JsonUtility_FromJson(game::sdk::Il2CppString *json, void *type)
{
	static auto o_fn = gsf::hooks::UnityEngine_JsonUtility_FromJson.get_original<decltype(hk_UnityEngine_JsonUtility_FromJson)>();
	return o_fn(json, type);
}

utils::hook_detour gsf::hooks::UnityEngine_JsonUtility_FromJson(hk_UnityEngine_JsonUtility_FromJson);