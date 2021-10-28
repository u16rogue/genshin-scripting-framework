#pragma once

#include "unity_component.h"

namespace game::sdk
{
	class Animator : public game::sdk::Component
	{
	public:
		float get_speed()
		{
			return this->_get_speed(this);
		}

	public:
		inline static game::sdk::unity_scripting_api<float, game::sdk::Animator *> _get_speed = ("UnityEngine.Animator::get_speed()");
	};
}

GSF_UNITY_SDK_ENSURE_NO_NONSTATIC(game::sdk::Animator);