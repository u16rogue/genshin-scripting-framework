#pragma once

#include <sol.hpp>
#include <memory>
#include <string>
#include <cstdint>
#include <filesystem>
#include "api/script_apis.h"

namespace gsf
{
	struct script_config
	{
		std::string name;
		std::string description;
	};

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

		bool load();
		bool unload();

		bool script_file_exists();
		operator bool() const;
		void load_mconfig();

		const std::string_view   get_filename() const;
		const gsf::script::state get_current_state() const;
		const gsf::script_config &get_config() const;

		const std::string_view   get_filepath() const override;
		sol::state              &get_lua_state() override;
		void                     script_push_log(std::string msg) override;

		static const char *state_to_cstr(script::state state_);

	public:
		mutable bool _tab_script_selected = false;
		mutable std::string _tab_script_notice = "None";

	private:
		gsf::script_config          config;
		const std::string           filepath;
		std::string                 filename;
		std::unique_ptr<sol::state> lua_state     = nullptr;
		script::state               current_state = script::state::UNLOADED;
	};
}