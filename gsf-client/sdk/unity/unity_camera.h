#pragma once

#include "unity_component.h"

namespace game::sdk
{
	class Camera : public game::sdk::Component
	{
	public:
		// pointless but...
		static game::sdk::Camera *get_main()
		{
			return game::sdk::Camera::_get_main();
		}

	public:
		inline static game::sdk::unity_scripting_api<game::sdk::Camera *> _get_main = ("UnityEngine.Camera::get_main()");
	};
}