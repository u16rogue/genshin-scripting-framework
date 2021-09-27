#pragma once

#include <sol.hpp>
#include <string>

namespace gsf
{
	class i_api
	{
	public:
		virtual sol::state &get_lua_state() = 0;
		virtual void script_push_log(std::string msg) = 0;
		virtual const std::string_view get_filepath() const = 0;
	};
}