#pragma once

#include <sol.hpp>
#include <memory>
#include <string>
#include <cstdint>
#include <hash.h>
#include <mutex>

namespace gsf
{
	/// <summary>
	/// Represents an instance of a script present in GSF
	/// </summary>
	class script
	{
	public:
		enum class state
		{
			UNLOADING,
			UNLOADED,
			LOADING,
			LOADED,
		};

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

		#define _GSF_SCRIPT_DECLARE_CALLBACK(name) script::callback name = script::callback(utils::hash_fnv1a(#name))
		struct callbacks_container
		{
			_GSF_SCRIPT_DECLARE_CALLBACK(on_imgui_draw);
			_GSF_SCRIPT_DECLARE_CALLBACK(dx_draw);
			_GSF_SCRIPT_DECLARE_CALLBACK(dx_drawindexed);
		};
		#undef _GSF_SCRIPT_DECLARE_CALLBACK

	public:
		inline static std::size_t count_loaded_scripts = 0;

	public:
		script(std::string_view filepath_);
		script(const script &) = default;

		bool load();
		bool unload();

		bool script_file_exists();
		operator bool() const;
		sol::state &get_lua() const;

		const std::vector<std::string>    &get_logs() const;
		const std::string_view             get_filepath() const;
		const gsf::script::state           get_current_state() const;
		const script::callbacks_container &get_callbacks() const;

	public:
		// Used for tracking the number of imgui.begin calls to automatically call ImGui::End. This is to prevent imgui
		// from crashing GSF when a script stops execution before properly calling imgui.iend
		mutable int imgui_active_begin_count = 0;

	private:
		const std::string           filepath;
		std::vector<std::string>    logs;
		std::unique_ptr<sol::state> lua_state     = nullptr;
		script::state               current_state = script::state::UNLOADED;
		script::callbacks_container callbacks;

	private:
		void internal_log_error(std::string_view msg);
		bool setup_script_api(sol::state &state);

	private:
		// ====================
		//  API IMPLEMENTATION
		// ====================

		// gsf
		void _api_gsf_log(std::string txt);
		bool _api_gsf_register_callback(std::string id, sol::function callback);

		// win
		sol::object _api_win_find_module(std::wstring module_name);

		// mem
		sol::object _api_mem_ida_scan(std::uintptr_t start_adr, std::size_t size, std::string ida_pattern);
		int _api_mem_patch(std::uintptr_t addr, std::vector<std::uint8_t> byte_array);
		std::uint64_t _api_mem_read_uint(std::uintptr_t addr, std::size_t prim_t_size);
		void _api_mem_write_uint(std::uintptr_t addr, std::size_t prim_t_size, std::uint64_t value);
		std::uintptr_t _api_mem_calc_rel_address_32(std::uintptr_t instruction_address, std::size_t instruction_operand_relative_offset);

		// imgui
		bool _api_imgui_begin(const char *text);
		void _api_imgui_iend();

		// player
		sol::table _api_player_get_map_coords();
	};
}