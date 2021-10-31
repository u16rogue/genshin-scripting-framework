#pragma once

#include "unity_component.h"

namespace game::sdk
{
	class Camera : public game::sdk::Component
	{
	public:
		inline static auto get_main = game::sdk::unity_setter_getter_static<game::sdk::Camera *>("UnityEngine.Camera::get_main()", nullptr);
	};
}

GSF_UNITY_SDK_ENSURE_NO_NONSTATIC(game::sdk::Camera);