#pragma once

#include <hash.h>
#include <sol.hpp>
#include <vector>
#include "script.h"

// For performance and lazy reasons
// it would be better to just keep the registered callbacks in the vector
// and just check their parent scripts if they're still loaded and active
// having to safely remove a registered callback in a vector would be too much
// effort when we can just keep it there just incase the script gets loaded again later
// not only will this eliminate the need of mutex locks and checks when clearing lua callbacks

namespace gsf::callback_manager
{
	struct api_callback
	{
		struct lua_callback
		{
			gsf::script *parent = nullptr;
			bool active = false;
			sol::function lua_func = sol::nil;

			bool valid()
			{
				return parent && parent->get_current_state() == gsf::script::state::LOADED && active;
			}
		};

		api_callback(const utils::fnv1a64_t _hashed_name)
			: hashed_name(_hashed_name) {}

		const utils::fnv1a64_t    hashed_name;
		std::vector<lua_callback> lua_callbacks;
		int                       active_callbacks_count = 0;

		template <typename... vargs_t>
		void dispatch(vargs_t... args) const
		{
			if (!this->active_callbacks_count)
				return;

			int total_dispatched = 0;
			for (auto &cb : this->lua_callbacks)
			{
				if (!cb.active)
					continue;

				cb.lua_func(args...);

				if (++total_dispatched == this->active_callbacks_count)
					return;
			}
		}

		template <typename... vargs_t>
		bool dispatch_cancellable(vargs_t... args) const
		{
			if (!this->active_callbacks_count)
				return false;

			int total_dispatched = 0;
			for (auto &cb : this->lua_callbacks)
			{
				if (!cb.active)
					continue;

				if (cb.lua_func(args...))
					return true;

				if (++total_dispatched == this->active_callbacks_count)
					return false;
			}

			return false;
		}
	};

	#define _GSF_DECLARE_API_CALLBACK(name) api_callback name = api_callback(utils::hash_fnv1a(#name));
	struct _api_callback_container
	{
		_GSF_DECLARE_API_CALLBACK(on_imgui_draw)
		_GSF_DECLARE_API_CALLBACK(dx_draw)
		_GSF_DECLARE_API_CALLBACK(dx_drawindexed)
		_GSF_DECLARE_API_CALLBACK(on_unload)
		_GSF_DECLARE_API_CALLBACK(menu_imgui_tab)
		_GSF_DECLARE_API_CALLBACK(on_key)
	};
	#undef _GSF_DECLARE_API_CALLBACK

	bool register_luafn(utils::fnv1a64_t hashed_name, gsf::script *parent, sol::function fn);
	void disable_api_callbacks_for_script(gsf::script *parent);
	const _api_callback_container &get_callbacks();
}