#pragma once

#include <sol.hpp>
#include <memory>
#include <string>

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

	private:
		const std::string filepath;
		std::vector<std::string> logs;
		std::unique_ptr<sol::state> lua_state = nullptr;


	private: // API IMPLEMENTATION
		void __internal_lua_api_gsf_log(std::string txt);

	};
}