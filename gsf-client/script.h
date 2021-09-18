#pragma once

#include <sol.hpp>
#include <memory>
#include <string>
#include <cstdint>
#include <hash.h>
#include <mutex>

#include "api/script_apis.h"

namespace gsf
{
	class script : public script_apis
	{
	public:
		enum class state
		{
			UNLOADING,
			UNLOADED,
			LOADING,
			LOADED,
		};

	public:
		inline static std::size_t count_loaded_scripts = 0;

	public:
		script(std::string_view filepath_);
		script(const script &) = default;

		bool load();
		bool unload();

		bool script_file_exists();
		operator bool() const;

		const std::string_view   get_filepath() const;
		const gsf::script::state get_current_state() const;

		sol::state &get_lua_state() override;

	private:
		const std::string           filepath;
		std::unique_ptr<sol::state> lua_state     = nullptr;
		script::state               current_state = script::state::UNLOADED;
	};
}