#pragma once

#include "i_api.h"

// TODO: refactor so we dont have to hardcode each case

namespace gsf
{
	class api_controls : public i_api
	{
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

		enum class direction : int
		{
			UP,
			DOWN,
			LEFT,
			RIGHT
		};

	protected:
		api_controls() {}
		bool setup_api(sol::state &slua);

	private:
		static void key_down(api_controls::keys key, int ticks);
		static void look(api_controls::direction direction, float amount);

	public:
		inline static int flag_mouse_left_down   = 0;
		inline static int flag_mouse_right_down  = 0;
		inline static int flag_mouse_middle_down = 0;

		inline static float flag_axis_x = 0.f;
		inline static float flag_axis_y = 0.f;
	};
}