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
			bool                active = false;
			sol::function       callback_function;
			mutable std::mutex  mutex;

			void reg(sol::function &function_);
			void unreg();
		};


		struct callbacks_container
		{
			#define _GSF_SCRIPT_DECLARE_CALLBACK(name) api_gsf::callback name = api_gsf::callback(utils::hash_fnv1a(#name));

			_GSF_SCRIPT_DECLARE_CALLBACK(on_imgui_draw)
			_GSF_SCRIPT_DECLARE_CALLBACK(dx_draw)
			_GSF_SCRIPT_DECLARE_CALLBACK(dx_drawindexed)

			#undef _GSF_SCRIPT_DECLARE_CALLBACK
		};

	protected:
		api_gsf() {};
		bool setup_api();

	public:
		const std::vector<std::string> &get_logs() const;
		const api_gsf::callbacks_container &get_callbacks() const;

	private:
		std::vector<std::string> logs;
		api_gsf::callbacks_container callbacks;

		void push_log(std::string msg) override;

	private:
		void _api_log(std::string txt);
		bool _api_register_callback(std::string id, sol::function callback);
	};
}