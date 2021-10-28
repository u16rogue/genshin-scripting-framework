#pragma once

#include "unity_component.h"

namespace game::sdk
{
	class Camera : public game::sdk::Component
	{
	public:
		inline static game::sdk::unity_scripting_api<game::sdk::Camera *> get_main = ("UnityEngine.Camera::get_main()");
	};
}

GSF_UNITY_SDK_ENSURE_NO_NONSTATIC(game::sdk::Camera);