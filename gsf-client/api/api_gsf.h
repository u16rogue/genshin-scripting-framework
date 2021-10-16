#pragma once

#include "i_api.h"

#include <vector>
#include <string>
#include <mutex>
#include <hash.h>

namespace gsf
{
	class api_gsf : public i_api
	{
	protected:
		api_gsf() {}
		bool setup_api(sol::state &slua);
		void script_push_log(std::string msg);

	private:
		void _api_log(std::string txt);
		bool _api_register_callback(std::string id, sol::function callback);
		std::string _api_get_script_dir();
		void _api_create_detached_thread(sol::function fn);
	};
}