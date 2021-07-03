#include "script.h"

#include <Windows.h>
#include <filesystem>

#include <macro.h>

gsf::script::script(std::string_view filepath_)
	: filepath(filepath_)
{
	DEBUG_COUT("\nConstructed gsf::script object # " << this->filepath << " # " << filepath_);
}

// TODO: use a "on_load" callback in the script instead of letting the script execute
bool gsf::script::load()
{
	this->logs.clear();

	if (!this->script_file_exists())
		return false;

	// We use a std::unique_ptr in the stack so incase of failure we can just immediately return and automatically destroy the lua state
	// and this->lua_state will remain invalid (we use the bool operator to validate a script instance which internally just checks if a lua state exists)
	std::unique_ptr<sol::state> temp_lua_state = std::make_unique<sol::state>();
	if (!temp_lua_state)
		return false;

	auto load_res = temp_lua_state->load_file(this->filepath);

	if (!load_res.valid())
	{
		sol::error err = load_res;
		DEBUG_COUT("\nLua load failure from: " << this->filepath << "\nReason: " << err.what());
		this->logs.push_back(err.what());
		return false;
	}

	// upon success we can move the unique_ptr from the stack to the class
	this->lua_state = std::move(temp_lua_state);

	// Start loading the API
	this->lua_state->set_function("gsf_log", &gsf::script::__internal_lua_api_gsf_log, this);

	DEBUG_COUT("\nLoaded lua: " << this->filepath);
	this->logs.push_back("Successfuly loaded.");

	// Run the script
	load_res();

	return true;
}

bool gsf::script::unload()
{
	if (!this->lua_state.operator bool())
		return false;

	this->logs.clear();

	DEBUG_COUT("\nUnloaded script: " << this->filepath);
	this->lua_state.reset();
	return true;
}

bool gsf::script::script_file_exists()
{
	return std::filesystem::exists(this->filepath);
}

const std::vector<std::string> &gsf::script::get_logs()
{
	return this->logs;
}

const std::string_view gsf::script::get_filepath()
{
	return this->filepath;
}

void gsf::script::__internal_lua_api_gsf_log(std::string txt)
{
	this->logs.push_back(txt);
}

gsf::script::operator bool() const
{
	return this->lua_state.operator bool();
}