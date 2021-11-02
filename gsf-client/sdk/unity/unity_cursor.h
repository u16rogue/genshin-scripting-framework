#pragma once

#include "unity_scripting_api.h"
#include "unity_primitive_types.h"

namespace game::sdk
{
	enum class CursorLockMode : int
	{
		None,
		Locked,
		Confined
	};

	class Cursor
	{
	public:
		inline static auto visible   = game::sdk::unity_setter_getter_static<game::sdk::Boolean>("UnityEngine.Cursor::get_visible()", "UnityEngine.Cursor::set_visible(System.Boolean)");
		inline static auto lockState = game::sdk::unity_setter_getter_static<game::sdk::CursorLockMode>("UnityEngine.Cursor::get_lockState()", "UnityEngine.Cursor::set_lockState(UnityEngine.CursorLockMode)");
	};
}

GSF_UNITY_SDK_ENSURE_NO_NONSTATIC(game::sdk::Cursor);