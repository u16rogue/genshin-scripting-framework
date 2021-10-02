#pragma once

#include "i_api.h"

namespace gsf
{
	class api_controls : public i_api
	{
	protected:
		api_controls() {}
		bool setup_api(sol::state &slua);

	private:
		static void mouse_down(int key, int ticks);

	public:
		inline static int flag_mouse_left_down   = 0;
		inline static int flag_mouse_right_down  = 0;
		inline static int flag_mouse_middle_down = 0;
	};
}