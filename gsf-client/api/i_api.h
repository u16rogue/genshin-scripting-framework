#pragma once

#include <sol.hpp>

namespace gsf
{
	class i_api
	{
	public:
		virtual sol::state &get_lua_state() = 0;
		virtual void push_log(std::string msg) = 0;
	};
}