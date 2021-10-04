#pragma once

#include "i_api.h"
#include <hash.h>

namespace gsf
{
	class api_controls : public i_api
	{
		template <typename enum_t, typename value_t>
		struct control_bridge_info
		{
			utils::fnv1a64_t game_identifier; // used for comparing from hooked unity engine scripting api
			enum_t           enum_tag; // used for comparing within GSF's api

			union
			{
				value_t amount;
				value_t ticks;
			};
		};

		enum class keys : int
		{
			LMOUSE = 0,
			ATTACK = LMOUSE,

			RMOUSE = 1,
			SPRINT = RMOUSE,

			MMOUSE          = 2,
			ELEMENTAL_SIGHT = MMOUSE,
			RESET_CAMERA    = MMOUSE,
		};

		enum class directions : int
		{
			UP    = 0b0001,
			DOWN  = 0b0011,
			RIGHT = 0b0100,
			LEFT  = 0b1100,

			X_AXIS = LEFT | RIGHT,
			Y_AXIS = UP   | DOWN,
		};

	protected:
		api_controls() {}
		bool setup_api(sol::state &slua);

	private:
		static void key_down(api_controls::keys key, int ticks);
		static void look(api_controls::directions direction, float amount);

	public:

		inline static api_controls::control_bridge_info<api_controls::keys, int> flags_mouse[] =
		{
			{ utils::hash_fnv1a_cv("MouseButton0"), api_controls::keys::LMOUSE, 0 },
			{ utils::hash_fnv1a_cv("MouseButton1"), api_controls::keys::RMOUSE, 0 },
			{ utils::hash_fnv1a_cv("MouseButton2"), api_controls::keys::MMOUSE, 0 },
		};

		inline static api_controls::control_bridge_info<api_controls::directions, float> flags_look[] =
		{
			{ utils::hash_fnv1a_cv("MouseAxis1"), api_controls::directions::X_AXIS, 0.f },
			{ utils::hash_fnv1a_cv("MouseAxis2"), api_controls::directions::Y_AXIS, 0.f },
		};
	};
}