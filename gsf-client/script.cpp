#include "script.h"

#include <Windows.h>
#include <filesystem>

gsf::script::script(std::string_view filepath_)
	: filepath(filepath_)
{
}

bool gsf::script::load()
{
	if (!this->script_file_exists())
	{
		std::cout << "\nFile " << this->filepath << " does not exist";
		return false;
	}

	this->lua_state = std::make_unique<sol::state>();
	if (!this->lua_state)
	{
		std::cout << "\nFailed to create lua state for script: " << this->filepath;
		return false;
	}

	auto load_res = this->lua_state->script_file(this->filepath);

	if (!load_res.valid())
	{
		sol::error load_err = load_res;
		std::cout << "\nFailed to load lua script from: " << this->filepath << "\n\nReason: " << load_err.what();
		this->lua_state.reset();
		return false;
	}

	return true;
}

bool gsf::script::unload()
{
	this->lua_state.reset();
	return true;
}

bool gsf::script::script_file_exists()
{
	return std::filesystem::exists(this->filepath);
}

std::string_view gsf::script::get_filepath()
{
	return this->filepath;
}

gsf::script::operator bool() const
{
	return this->lua_state.operator bool();
}
