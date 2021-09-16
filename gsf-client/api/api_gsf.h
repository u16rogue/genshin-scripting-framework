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
		struct callback
		{
			callback(std::uint64_t hashed_name_)
				: hashed_name(hashed_name_) {}

			const std::uint64_t hashed_name;
			bool active = false;
			sol::function callback_function;
			mutable std::mutex mutex;

			void reg(sol::function &function_)
			{
				this->callback_function = function_;
				this->active = true;
			};

			void unreg()
			{
				this->callback_function = sol::nil;
				this->active = false;
			};
		};

		#define _GSF_SCRIPT_DECLARE_CALLBACK(name) api_gsf::callback name = api_gsf::callback(utils::hash_fnv1a(#name))
		struct callbacks_container
		{
			_GSF_SCRIPT_DECLARE_CALLBACK(on_imgui_draw);
			_GSF_SCRIPT_DECLARE_CALLBACK(dx_draw);
			_GSF_SCRIPT_DECLARE_CALLBACK(dx_drawindexed);
		};
		#undef _GSF_SCRIPT_DECLARE_CALLBACK

	public:
		api_gsf();

		std::vector<std::string> logs;
		api_gsf::callbacks_container callbacks;

	private:
		bool setup_api();
		void push_log(std::string msg) override;

	private:
		void _api_log(std::string txt);
		bool _api_register_callback(std::string id, sol::function callback);
	};
}