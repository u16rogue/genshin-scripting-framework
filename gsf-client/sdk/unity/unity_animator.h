#pragma once

#include "unity_component.h"

namespace game::sdk
{
	struct Animator_Unknown0
	{
	private:
		char pad0[0x314];
	public:
		float speed;
	};

	class Animator : public game::sdk::Component
	{
	private:
		char pad0[0x10];
	public:
		Animator_Unknown0 *unk0;

	public:
		float get_speed()
		{
			return this->_get_speed(this);
		}

	public:
		inline static game::sdk::unity_scripting_api<float, game::sdk::Animator *> _get_speed = ("UnityEngine.Animator::get_speed()");
	};
}

static_assert(offsetof(game::sdk::Animator, unk0) == 0x10, "Offset mismatch for Animator::unk0!");
static_assert(offsetof(game::sdk::Animator_Unknown0, speed) == 0x314, "Offset mismatch for Animator_Unknown0::speed");