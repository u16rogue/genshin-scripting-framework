#include "script.h"

#include <Windows.h>
#include <macro.h>
#include "log_manager.h"
#include "callback_manager.h"

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
	this->filename = this->filepath.substr(this->filepath.find_last_of("/\\") + 1);
	this->load_mconfig();
}

bool gsf::script::load()
{
	if (!this->script_file_exists())
		return false;

	_load_state_trigger state_trigger(this->current_state);

	// We use a std::unique_ptr in the stack so incase of failure we can just immediately return and automatically destroy the lua state
	// and this->lua_state will remain invalid (we use the bool operator to validate a script instance which internally just checks if a lua state exists)
	std::unique_ptr<sol::state> temp_lua_state = std::make_unique<sol::state>();
	if (!temp_lua_state)
		return false;

	temp_lua_state->open_libraries(sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::base, sol::lib::bit32, sol::lib::coroutine, sol::lib::io, sol::lib::os, sol::lib::package);

	if (auto load_res = temp_lua_state->script_file(this->filepath); !load_res.valid())
	{
		sol::error err_load_res = load_res;
		gsf::log_manager::push_log(err_load_res.what(), gsf::log_manager::log_type::LUA);
		return false;
	}

	sol::function callback_onload;
	{
		if (auto get_onload_res = temp_lua_state->operator[]("on_load"); get_onload_res.valid())
		{
			callback_onload = get_onload_res;
		}
		else
		{
			gsf::log_manager::push_log("Callback \"on_load\" not found.", gsf::log_manager::log_type::GSF);
			return false;
		}
	}

	if (!gsf::script_apis::setup_all_apis(*temp_lua_state.get()))
	{
		gsf::log_manager::push_log("Failed to load API in lua state", gsf::log_manager::log_type::GSF);
		return false;
	}

	// upon success we can move the unique_ptr from this scope to the class
	this->lua_state = std::move(temp_lua_state);
	state_trigger.set_state_loaded();
	++gsf::script::count_loaded_scripts;

	DEBUG_COUT("\nLoaded lua: " << this->filepath);
	gsf::log_manager::push_log("Script Loaded: " + this->filename, gsf::log_manager::log_type::GSF);

	// Load configs
	this->load_mconfig();

	// Run the script
	callback_onload();
	return true;
}

bool gsf::script::unload()
{
	if (!this->lua_state.operator bool())
		return false;

	this->current_state = script::state::UNLOADING;

	gsf::callback_manager::get_callbacks().on_unload.dispatch_filtered(this); // this is inefficient ik
	gsf::callback_manager::disable_api_callbacks_for_script(this);

	this->lua_state.reset();
	this->current_state = gsf::script::state::UNLOADED;
	--gsf::script::count_loaded_scripts;

	DEBUG_COUT("\nUnloaded script: " << this->filepath);
	gsf::log_manager::push_log("Script Unloaded: " + this->filename, gsf::log_manager::log_type::GSF);
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

void gsf::script::load_mconfig()
{
	sol::state temp_state;
	sol::state *sel_state = this->lua_state.get(); // load the lua state of the instance
	// check if there's a lua state, if there's not one yet then use a temporary state;
	if (!this->lua_state)
	{
		temp_state.script_file(this->filepath);
		sel_state = &temp_state;
	}
	sol::state &_l = *sel_state;

	#define _GSF_LOAD_CONFIG_CTMDEF(name, def_val) this->config. ## name ## = _l["_mconfig"][#name].get_or(def_val)
	#define _GSF_LOAD_CONFIG(name) _GSF_LOAD_CONFIG_CTMDEF(name, this->config.##name##)

	_GSF_LOAD_CONFIG_CTMDEF(name, this->get_filename());
	_GSF_LOAD_CONFIG(description);

	#undef _GSF_LOAD_CONFIG
	#undef _GSF_LOAD_CONFIG_CTMDEF
}

const std::string_view gsf::script::get_filepath() const
{
	return this->filepath;
}

const std::string_view gsf::script::get_filename() const
{
	return this->filename;
}

const gsf::script::state gsf::script::get_current_state() const
{
	return this->current_state;
}

const gsf::script_config &gsf::script::get_config() const
{
	return this->config;
}

sol::state &gsf::script::get_lua_state()
{
	return *this->lua_state.get();
}

void gsf::script::script_push_log(std::string msg)
{
	api_gsf::script_push_log(std::move(msg));
}

#define _CASE_STATE_TO_RET_STR(state_) case gsf::script::state:: ## state_ ## : return #state_
const char *gsf::script::state_to_cstr(script::state state_)
{
	switch (state_)
	{
		_CASE_STATE_TO_RET_STR(UNLOADING);
		_CASE_STATE_TO_RET_STR(UNLOADED);
		_CASE_STATE_TO_RET_STR(LOADING);
		_CASE_STATE_TO_RET_STR(LOADED);
	}

	return nullptr;
}
#undef _CASE_STATE_TO_RET_STR