#pragma once

#include <sol.hpp>

namespace gsf
{
	class i_api
	{
	protected:
		i_api() {}
	public:
		bool setup_api(sol::state &slua) { return false; };
		virtual sol::state &get_lua_state() = 0;
		virtual void push_log(std::string msg) = 0;
	};
}