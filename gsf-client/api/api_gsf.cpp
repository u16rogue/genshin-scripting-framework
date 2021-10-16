#include "api_gsf.h"
#include <macro.h>
#include "../log_manager.h"
#include <filesystem>
#include "../callback_manager.h"

bool gsf::api_gsf::setup_api(sol::state &slua)
{
	auto namespace_gsf = slua["gsf"].get_or_create<sol::table>();
	namespace_gsf.set_function("log", &api_gsf::_api_log, this);
	namespace_gsf.set_function("register_callback", &api_gsf::_api_register_callback, this);
	namespace_gsf.set_function("get_script_dir", &api_gsf::_api_get_script_dir, this);
	namespace_gsf.set_function("create_detached_thread", &api_gsf::_api_create_detached_thread, this);

	return true;
}

void gsf::api_gsf::script_push_log(std::string msg)
{
	DEBUG_COUT("\n[SCRIPT] " << msg);
	gsf::log_manager::push_log(std::move(msg), gsf::log_manager::log_type::SCRIPTS);
}

void gsf::api_gsf::_api_log(std::string txt)
{
	this->script_push_log(std::move(txt));
}

bool gsf::api_gsf::_api_register_callback(std::string id, sol::function callback)
{
	return gsf::callback_manager::register_luafn(utils::hash_fnv1a(id.c_str()), this, callback);
}

std::string gsf::api_gsf::_api_get_script_dir()
{
	auto abs = std::filesystem::canonical(this->get_filepath()).string();
	return abs.substr(0, abs.find_last_of("/\\") + 1);
}

void gsf::api_gsf::_api_create_detached_thread(sol::function fn)
{
	return std::thread([](sol::function fn)
	{
		fn();
	}, std::move(fn)).detach();
}
