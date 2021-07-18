#pragma once

#include <sol.hpp>
#include <memory>
#include <string>
#include <cstdint>

namespace gsf
{
	/// <summary>
	/// Represents an instance of a script present in GSF
	/// </summary>
	class script
	{
	public:
		script(std::string_view filepath_);

		bool load();
		bool unload();

		bool script_file_exists();
		operator bool() const;

		const std::vector<std::string> &get_logs();
		const std::string_view get_filepath();

	public:
		inline static std::size_t count_loaded_scripts = 0;

	private:
		const std::string filepath;
		std::vector<std::string> logs;
		std::unique_ptr<sol::state> lua_state = nullptr;

	private:
		void internal_log_error(std::string_view msg);
		bool setup_script_api(std::unique_ptr<sol::state> &state);

	private:
		// ====================
		//  API IMPLEMENTATION
		// ====================

		// gsf
		void _api_gsf_log(std::string txt);

		// winternal
		sol::table _api_win_find_module(std::wstring mod_name);
	};
}