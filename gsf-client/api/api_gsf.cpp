#include "api_gsf.h"
#include <macro.h>
#include "../log_manager.h"

void gsf::callback_api::reg(sol::function &function_)
{
	this->callback_function = function_;
	this->active = true;
};

void gsf::callback_api::unreg()
{
	this->callback_function = sol::nil;
	this->active = false;
};

bool gsf::api_gsf::setup_api(sol::state &slua)
{
	auto namespace_gsf = slua["gsf"].get_or_create<sol::table>();
	namespace_gsf.set_function("log", &api_gsf::_api_log, this);
	namespace_gsf.set_function("register_callback", &api_gsf::_api_register_callback, this);

	return true;
}

void gsf::api_gsf::clear_callbacks()
{
	gsf::callback_api *arr_callbacks = reinterpret_cast<gsf::callback_api *>(&this->callbacks);
	for (int i = 0; i < sizeof(this->callbacks) / sizeof(gsf::callback_api); ++i)
	{
		if (auto &callback = arr_callbacks[i]; callback.active)
		{
			const std::lock_guard<std::mutex> lock(callback.mutex);
			callback.unreg();
		}
	}
}

const gsf::callbacks_container &gsf::api_gsf::get_callbacks() const
{
	return this->callbacks;
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
	auto hashed_id = utils::hash_fnv1a(id.c_str());
	gsf::callback_api *arr_callbacks = reinterpret_cast<gsf::callback_api *>(&this->callbacks);
	for (int i = 0; i < sizeof(this->callbacks) / sizeof(callback); ++i)
	{
		if (arr_callbacks[i].active == false && hashed_id == arr_callbacks[i].hashed_name)
		{
			arr_callbacks[i].reg(callback);
			return true;
		}
	}

	gsf::log_manager::push_log("Invalid callback ID: " + id, gsf::log_manager::log_type::GSF);
	return false;
}