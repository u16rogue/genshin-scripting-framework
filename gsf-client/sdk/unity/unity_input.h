#pragma once

#include "unity_scripting_api.h"
#include "unity_primitive_types.h"

namespace game::sdk
{
	class Input
	{
	public:
		inline static game::sdk::unity_scripting_api<game::sdk::Boolean, game::sdk::Il2CppString *> GetButton  = ("UnityEngine.Input::GetButton(System.String)");
		inline static game::sdk::unity_scripting_api<float, game::sdk::Il2CppString *>              GetAxisRaw = ("UnityEngine.Input::GetAxisRaw(System.String)");
	};
}