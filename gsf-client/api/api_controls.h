#pragma once

#include "i_api.h"

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

	protected:
		api_controls() {}
		bool setup_api(sol::state &slua);

	private:
		static void mouse_down(api_controls::keys key, int ticks);

	public:
		inline static int flag_mouse_left_down   = 0;
		inline static int flag_mouse_right_down  = 0;
		inline static int flag_mouse_middle_down = 0;
	};
}