#pragma once

#include "unity_object.h"

namespace game::sdk
{
	class Animator : public game::sdk::Object
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