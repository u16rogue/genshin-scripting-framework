#pragma once

#include "api_game.h"
#include "api_gsf.h"
#include "api_imgui.h"
#include "api_mem.h"
#include "api_win.h"

namespace gsf
{
	template <class... components>
	class _script_apis : public components...
	{
	protected:
		_script_apis() {}

		bool setup_all_apis(sol::state &slua)
		{
			return (components::setup_api(slua) && ...);
		}
	};

	using script_apis = _script_apis<
		api_gsf,
		api_game,
		api_imgui,
		api_mem,
		api_win
	>;
}