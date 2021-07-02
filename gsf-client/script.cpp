#include "script.h"

#include <Windows.h>
#include <filesystem>

#include <macro.h>

gsf::script::script(std::string_view filepath_)
	: filepath(filepath_)
{
	DEBUG_COUT("\nConstructed gsf::script object # " << this->filepath << " # " << filepath_);
}

bool gsf::script::load()
{
	if (!this->script_file_exists())
		return false;

	// We use a std::unique_ptr in the stack so incase of failure we can just immediately return and automatically destroy the lua state
	// and this->lua_state will remain invalid (we use the bool operator to validate a script instance which internally just checks if a lua state exists)
	std::unique_ptr<sol::state> temp_lua_state = std::make_unique<sol::state>();
	if (!temp_lua_state)
		return false;

	auto load_res = temp_lua_state->script_file(this->filepath);

	if (!load_res.valid())
	{
		sol::error err = load_res;
		DEBUG_COUT("\nLua load failure from: " << this->filepath << "\nReason: " << err.what());
		return false;
	}

	this->lua_state = std::move(temp_lua_state); // upon success we can move the unique_ptr from the stack to the class
	return true;
}

bool gsf::script::unload()
{
	DEBUG_COUT("\nUnloaded script: " << this->filepath);
	this->lua_state.reset();
	return true;
}

bool gsf::script::script_file_exists()
{
	return std::filesystem::exists(this->filepath);
}

const std::string_view gsf::script::get_filepath()
{
	return this->filepath;
}

gsf::script::operator bool() const
{
	return this->lua_state.operator bool();
}
