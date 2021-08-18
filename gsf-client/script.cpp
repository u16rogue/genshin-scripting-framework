#include "script.h"

#include <Windows.h>
#include <filesystem>
#include <winternal.h>
#include <macro.h>
#include <pattern_scan.h>
#include <imgui.h>

/// <summary>
/// RAII implementation of applying script state value
/// Reasoning: im not gonna type [this->state_value = gsf::script::state::UNLOADED;] for every [return false;] in gsf::script::load()
/// </summary>
class _load_state_trigger
{
public:
	_load_state_trigger(gsf::script::state &state_value_)
		: state_value(state_value_)
	{
		this->state_value = gsf::script::state::LOADING;
	}

	~_load_state_trigger()
	{
		if (this->state_value == gsf::script::state::LOADING)
			this->state_value = gsf::script::state::UNLOADED;
	}

	void set_state_loaded()
	{
		this->state_value = gsf::script::state::LOADED;
	}

private:
	gsf::script::state &state_value;
};

gsf::script::script(std::string_view filepath_)
	: filepath(filepath_)
{
	DEBUG_COUT("\nConstructed gsf::script object # " << this->filepath << " # " << filepath_);
}

bool gsf::script::load()
{
	this->logs.clear();

	if (!this->script_file_exists())
		return false;

	_load_state_trigger state_trigger(this->current_state);

	// We use a std::unique_ptr in the stack so incase of failure we can just immediately return and automatically destroy the lua state
	// and this->lua_state will remain invalid (we use the bool operator to validate a script instance which internally just checks if a lua state exists)
	std::unique_ptr<sol::state> temp_lua_state = std::make_unique<sol::state>();
	if (!temp_lua_state)
		return false;

	temp_lua_state->open_libraries(sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::base, sol::lib::bit32, sol::lib::coroutine);

	auto load_res = temp_lua_state->script_file(this->filepath);

	if (!load_res.valid())
	{
		sol::error err_load_res = load_res;
		this->internal_log_error(err_load_res.what());
		return false;
	}

	sol::function callback_onload;
	{
		auto get_onload_res = temp_lua_state->operator[]("on_load");
		if (get_onload_res.valid())
		{
			callback_onload = get_onload_res;
		}
		else
		{
			this->internal_log_error("Callback \"on_load\" not found.");
			return false;
		}
	}

	if (!gsf::script::setup_script_api(*temp_lua_state.get()))
	{
		this->internal_log_error("Failed to load API in lua state");
		return false;
	}

	// upon success we can move the unique_ptr from this scope to the class
	this->lua_state = std::move(temp_lua_state);
	state_trigger.set_state_loaded();
	++gsf::script::count_loaded_scripts;

	DEBUG_COUT("\nLoaded lua: " << this->filepath);
	this->logs.push_back("Successfuly loaded.");

	// Run the script
	callback_onload();
	return true;
}

bool gsf::script::unload()
{
	if (!this->lua_state.operator bool())
		return false;

	this->current_state = script::state::UNLOADING;

	script::callback *arr_callbacks = reinterpret_cast<script::callback *>(&this->callbacks);
	for (int i = 0; i < sizeof(this->callbacks) / sizeof(callback); ++i)
	{
		if (auto &callback = arr_callbacks[i]; callback.active)
		{
			const std::lock_guard<std::mutex> lock(callback.mutex);
			callback.unreg();
		}
	}

	this->logs.clear();
	this->lua_state.reset();
	this->current_state = gsf::script::state::UNLOADED;
	--gsf::script::count_loaded_scripts;

	DEBUG_COUT("\nUnloaded script: " << this->filepath);
	return true;
}

bool gsf::script::script_file_exists()
{
	return std::filesystem::exists(this->filepath);
}

gsf::script::operator bool() const
{
	return this->lua_state.operator bool();
}

const std::vector<std::string> &gsf::script::get_logs() const
{
	return this->logs;
}

const std::string_view gsf::script::get_filepath() const
{
	return this->filepath;
}

const gsf::script::state gsf::script::get_current_state() const
{
	return this->current_state;
}

sol::state &gsf::script::get_lua() const
{
	return *this->lua_state.get();
}

const gsf::script::callbacks_container &gsf::script::get_callbacks() const
{
	return this->callbacks;
}

void gsf::script::internal_log_error(std::string_view msg)
{
	DEBUG_COUT("\nLua failure from: " << this->filepath << "\nReason: " << msg);
	this->logs.emplace_back(msg.data());
}

bool gsf::script::setup_script_api(sol::state &state)
{
	// gsf namespace
	auto namespace_gsf = state["gsf"].get_or_create<sol::table>();
	namespace_gsf.set_function("log", &gsf::script::_api_gsf_log, this);
	namespace_gsf.set_function("register_callback", &gsf::script::_api_gsf_register_callback, this);

	// win namespace
	auto namespace_win = state["win"].get_or_create<sol::table>();
	namespace_win.set_function("find_module", &gsf::script::_api_win_find_module, this);

	// mem namespace
	auto namespace_mem = state["mem"].get_or_create<sol::table>();
	namespace_mem.set_function("ida_scan", &gsf::script::_api_mem_ida_scan, this);
	namespace_mem.set_function("patch", &gsf::script::_api_mem_patch, this);
	namespace_mem.set_function("read_uint", &gsf::script::_api_mem_read_uint, this);
	namespace_mem.set_function("write_uint", &gsf::script::_api_mem_write_uint, this);

	// imgui namespace
	auto namespace_imgui = state["imgui"].get_or_create<sol::table>();
	namespace_imgui.set_function("begin", [](const char *text) -> bool { return ImGui::Begin(text, nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse); });
	namespace_imgui.set_function("iend", &ImGui::End);
	namespace_imgui.set_function("text", [](const char *text) { ImGui::Text(text); } );
	namespace_imgui.set_function("same_line", []() { ImGui::SameLine(); } );
	namespace_imgui.set_function("button", [](const char *text) -> bool { return ImGui::Button(text); });
	namespace_imgui.set_function("separator", &ImGui::Separator);

	return true;
}

void gsf::script::_api_gsf_log(std::string txt)
{
	DEBUG_COUT("\n[SCRIPT] " << txt);
	this->logs.push_back(txt);
}

bool gsf::script::_api_gsf_register_callback(std::string id, sol::function callback)
{
	auto hashed_id = utils::hash_fnv1a(id.c_str());
	script::callback *arr_callbacks = reinterpret_cast<script::callback *>(&this->callbacks);
	for (int i = 0; i < sizeof(this->callbacks) / sizeof(callback); ++i)
	{
		if (arr_callbacks[i].active == false && hashed_id == arr_callbacks[i].hashed_name)
		{
			arr_callbacks[i].reg(callback);
			return true;
		}
	}

	this->internal_log_error("Invalid callback ID: " + id);
	return false;
}

sol::object gsf::script::_api_win_find_module(std::wstring module_name)
{
	auto ldr = utils::ldr_data_table_entry_find(module_name.c_str());

	if (ldr)
		return this->lua_state->create_table_with("base_address", reinterpret_cast<std::uintptr_t>(ldr->dll_base), "size", ldr->size_of_image);

	return sol::nil;
}

std::uintptr_t gsf::script::_api_mem_ida_scan(std::uintptr_t start_adr, std::size_t size, std::string ida_pattern)
{
	return reinterpret_cast<std::uintptr_t>(utils::ida_scan(reinterpret_cast<void *>(start_adr), size, ida_pattern.c_str()));
}

int gsf::script::_api_mem_patch(std::uintptr_t addr, std::vector<std::uint8_t> byte_array)
{
	enum result_e
	{
		SUCCESSFUL = 0,
		ALREADY_PATCHED = 1,
		PROT_CHANGE_XRW_FAILED = 2,
		PROT_CHANGE_RESTORE_FAILED = 3
	};

	// TODO: store in a global static list all the patched memory and check for collisions
	// TODO: use win api helper

	void *addr_void = reinterpret_cast<void *>(addr);
	std::size_t byte_arr_size = byte_array.size();
	DWORD old_prot = 0;

	if (!VirtualProtect(addr_void, byte_arr_size, PAGE_EXECUTE_READWRITE, &old_prot))
	{
		this->internal_log_error("mem.patch # exception: PROT_CHANGE_XRW_FAILED");
		return result_e::PROT_CHANGE_XRW_FAILED;
	}

	std::memcpy(addr_void, byte_array.data(), byte_arr_size);

	if (!VirtualProtect(addr_void, byte_arr_size, old_prot, &old_prot))
	{
		this->internal_log_error("mem.patch # exception: PROT_CHANGE_RESTORE_FAILED");
		return result_e::PROT_CHANGE_RESTORE_FAILED;
	}

	return result_e::SUCCESSFUL;
}

std::uint64_t gsf::script::_api_mem_read_uint(std::uintptr_t addr, std::size_t prim_t_size)
{
	if (prim_t_size > 8)
	{
		this->internal_log_error("mem.read_uint # parameter prim_t_size is out of bound");
		return -1;
	}

	std::uint64_t result = 0;
	std::memcpy(&result, reinterpret_cast<void *>(addr), prim_t_size);
	return result;
}

void gsf::script::_api_mem_write_uint(std::uintptr_t addr, std::size_t prim_t_size, std::uint64_t value)
{
	if (prim_t_size > 8)
	{
		this->internal_log_error("mem.write_uint # parameter prim_t_size is out of bound");
	}

	std::memcpy(reinterpret_cast<void *>(addr), &value, prim_t_size);
}