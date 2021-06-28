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

		const std::string_view get_filepath();

		operator bool() const;

	private:
		const std::string filepath;
		std::unique_ptr<sol::state> lua_state;
	};
}