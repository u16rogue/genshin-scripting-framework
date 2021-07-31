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

		// win
		sol::object _api_win_find_module(std::wstring module_name);

		// mem
		std::uintptr_t _api_mem_ida_scan(std::uintptr_t start_adr, std::size_t size, std::string ida_pattern);
		int _api_mem_patch(std::uintptr_t addr, std::vector<std::uint8_t> byte_array);
		std::uint64_t _api_mem_read_uint(std::uintptr_t addr, std::size_t prim_t_size);
		void _api_mem_write_uint(std::uintptr_t addr, std::size_t prim_t_size, std::uint64_t value);
	};
}