#pragma once

#include "unity_component.h"
#include "unity_primitive_types.h"

namespace game::sdk
{
	union Animator_class_unk0
	{
		struct
		{
			char _pad0[0x0F0];
			float *pspeed; // speed value to use by the engine
		};
	};

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

		struct
		{
			char _pad2[0x0EC];
			game::sdk::Boolean unk0; // unknown boolean, this is checked before doing speed value assignment
		};

		struct
		{
			char _pad3[0x4A0];
			Animator_class_unk0 *unk1;
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