#pragma once

#include "unity_scripting_api.h"

namespace game::sdk
{
	class Object
	{
	public:
		game::sdk::Il2CppString *get_name()
		{
			return this->_get_name(this);
		}

		game::sdk::Il2CppString *ToString()
		{
			return this->_ToString(this);
		}

	public:
		inline static game::sdk::unity_scripting_api<game::sdk::Il2CppString *, game::sdk::Object *> _get_name = ("UnityEngine.Object::get_name()");
		inline static game::sdk::unity_scripting_api<game::sdk::Il2CppString *, game::sdk::Object *> _ToString = ("UnityEngine.Object::ToString()");
	};
}

GSF_UNITY_SDK_ENSURE_NO_NONSTATIC(game::sdk::Object);