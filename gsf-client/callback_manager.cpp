#include "callback_manager.h"
#include <mutex>

static std::mutex mut_active_cb_count_update;
static gsf::callback_manager::_api_callback_container api_callbacks;

constexpr int n_api_callbacks = sizeof(gsf::callback_manager::_api_callback_container) / sizeof(gsf::callback_manager::api_callback);

const gsf::callback_manager::_api_callback_container &gsf::callback_manager::get_callbacks()
{
	return api_callbacks;
}

bool gsf::callback_manager::register_luafn(utils::fnv1a64_t hashed_name, gsf::script *parent, sol::function fn)
{

	for (int i = 0; i < n_api_callbacks; i++)
	{
		auto &cb_api = reinterpret_cast<gsf::callback_manager::api_callback *>(&api_callbacks)[i];

		if (cb_api.hashed_name != hashed_name)
			continue;

		// check if it was registered previously
		for (auto &lua_api : cb_api.lua_callbacks)
		{
			if (lua_api.parent != parent)
				continue;

			if (lua_api.active)
				return false; // fail cb register if already active

			// update lua callback and set it back to active
			lua_api.lua_func = fn;
			lua_api.active = true;
			++cb_api.active_callbacks_count;
			return true;
		}

		// for newly registered callbacks
		cb_api.lua_callbacks.emplace_back(parent, true, std::move(fn));

		const std::lock_guard lg(mut_active_cb_count_update);
		++cb_api.active_callbacks_count;

		return true;
	}

	return false;
}

void gsf::callback_manager::disable_api_callbacks_for_script(gsf::script *parent)
{
	for (int i = 0; i < n_api_callbacks; i++)
	{
		auto &cb_api = reinterpret_cast<gsf::callback_manager::api_callback *>(&api_callbacks)[i];

		for (auto &lua_api : cb_api.lua_callbacks)
		{
			if (lua_api.parent != parent)
				continue;

			lua_api.lua_func = sol::nil;
			lua_api.active = false;

			const std::lock_guard lg(mut_active_cb_count_update);
			--cb_api.active_callbacks_count;
		}
	}
}
