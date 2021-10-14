#pragma once

#include "i_api.h"

#include <vector>
#include <string>
#include <mutex>
#include <hash.h>

namespace gsf
{
	struct callback_api
	{
		callback_api(std::uint64_t hashed_name_)
			: hashed_name(hashed_name_) {}

		const std::uint64_t hashed_name;
		bool                active = false;
		sol::function       callback_function;
		mutable std::mutex  mutex;

		void reg(sol::function &function_);
		void unreg();
	};


	struct callbacks_container
	{
		#define _GSF_SCRIPT_DECLARE_CALLBACK(name) callback_api name = callback_api(utils::hash_fnv1a(#name));

		_GSF_SCRIPT_DECLARE_CALLBACK(on_imgui_draw)
		_GSF_SCRIPT_DECLARE_CALLBACK(dx_draw)
		_GSF_SCRIPT_DECLARE_CALLBACK(dx_drawindexed)
		_GSF_SCRIPT_DECLARE_CALLBACK(on_unload)
		_GSF_SCRIPT_DECLARE_CALLBACK(menu_imgui_tab)
		_GSF_SCRIPT_DECLARE_CALLBACK(on_key)

		#undef _GSF_SCRIPT_DECLARE_CALLBACK
	};

	class api_gsf : public i_api
	{
	protected:
		api_gsf() {}
		bool setup_api(sol::state &slua);
		callbacks_container callbacks;
		void script_push_log(std::string msg);

	public:
		void clear_callbacks();
		const callbacks_container &get_callbacks() const;

	private:
		void _api_log(std::string txt);
		bool _api_register_callback(std::string id, sol::function callback);
		std::string _api_get_script_dir();
		void _api_create_detached_thread(sol::function fn);
	};
}