#pragma once

#include "unity_scripting_api.h"
#include "unity_primitive_types.h"

namespace game::sdk
{
	class Cursor
	{
	public:
		inline static game::sdk::unity_scripting_api<game::sdk::Boolean>              get_visible   = ("UnityEngine.Cursor::get_visible()");
		inline static game::sdk::unity_scripting_api<void, game::sdk::Boolean>        set_visible   = ("UnityEngine.Cursor::set_visible(System.Boolean)");
		inline static game::sdk::unity_scripting_api<game::sdk::CursorLockMode>       get_lockState = ("UnityEngine.Cursor::get_lockState()");
		inline static game::sdk::unity_scripting_api<void, game::sdk::CursorLockMode> set_lockState = ("UnityEngine.Cursor::set_lockState(UnityEngine.CursorLockMode)");
	};
}