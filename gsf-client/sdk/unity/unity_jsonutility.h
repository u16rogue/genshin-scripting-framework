#pragma once

#include "unity_scripting_api.h"
#include "unity_object.h"
#include "unity_primitive_types.h"

namespace game::sdk
{
	class JsonUtility
	{
	public:
		inline static game::sdk::unity_scripting_api<game::sdk::Il2CppString *, game::sdk::Object *, game::sdk::Boolean> ToJson            = ("UnityEngine.JsonUtility::ToJson(System.Object,System.Boolean)");
		inline static game::sdk::unity_scripting_api<game::sdk::Object *, game::sdk::Il2CppString *, game::sdk::Type *>  FromJson          = ("UnityEngine.JsonUtility::FromJson(System.String,System.Type)");
		inline static game::sdk::unity_scripting_api<void, game::sdk::Il2CppString *, game::sdk::Object *>               FromJsonOverwrite = ("UnityEngine.JsonUtility::FromJsonOverwrite(System.String,System.Object)");
	};
}