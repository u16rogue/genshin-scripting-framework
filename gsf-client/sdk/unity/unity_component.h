#pragma once

#include "unity_object.h"
#include "unity_transform.h"

namespace game::sdk
{
	class Component : public game::sdk::Object
	{
	public:
		game::sdk::Transform *get_transform()
		{
			return this->_get_transform(this);
		}

	public:
		inline static game::sdk::unity_scripting_api<game::sdk::Transform *, game::sdk::Component *> _get_transform = ("UnityEngine.Component::get_transform()");
	};
}