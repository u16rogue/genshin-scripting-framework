#pragma once

#include "unity_component.h"

namespace game::sdk
{
	class Animator;

	union AnimatorStateInfo_MAYBE
	{
		struct
		{
			char _pad0[0x28];
			Animator *parent;
		};

		struct
		{
			char _pad1[0x314];
			float speed;
		};
	};


	// NOTE: this is probably the boxed AnimatorStateInfo__Boxed that is being passed into animator get speed and the actual animator is Animator_Unkown0
	class Animator : public game::sdk::Component
	{
	private:
		char pad0[0x10];
	public:
		AnimatorStateInfo_MAYBE *state_info;

	public:
		float get_speed()
		{
			return this->_get_speed(this);
		}

	public:
		inline static game::sdk::unity_scripting_api<float, game::sdk::Animator *> _get_speed = ("UnityEngine.Animator::get_speed()");
	};
}

static_assert(offsetof(game::sdk::Animator, state_info) == 0x10, "Offset mismatch for Animator::state_info!");
static_assert(offsetof(game::sdk::AnimatorStateInfo_MAYBE, speed) == 0x314, "Offset mismatch for AnimatorStateInfo_MAYBE::speed");
static_assert(offsetof(game::sdk::AnimatorStateInfo_MAYBE, parent) == 0x28, "Offset mismatch for AnimatorStateInfo_MAYBE::parent");