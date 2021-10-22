#pragma once

#include "unity_object.h"
#include "unity_vector3.h"

namespace game::sdk
{
	class Transform : public game::sdk::Object
	{
	public:
		void get_position(game::sdk::Vector3 &position_out)
		{
			this->_INTERNAL_get_position(this, &position_out);
		}

	public:
		inline static game::sdk::unity_scripting_api<void, game::sdk::Transform *, game::sdk::Vector3 *> _INTERNAL_get_position = ("UnityEngine.Transform::INTERNAL_get_position(UnityEngine.Vector3&)");
	};
}