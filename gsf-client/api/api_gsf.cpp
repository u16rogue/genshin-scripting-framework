#include "api_gsf.h"

gsf::api_gsf::api_gsf()
{
}

bool gsf::api_gsf::setup_api()
{
	auto namespace_gsf = this->get_lua_state()["gsf"].get_or_create<sol::table>();
	namespace_gsf.set_function("log", &api_gsf::_api_log, this);
	namespace_gsf.set_function("register_callback", &api_gsf::_api_register_callback, this);

	return true;
}

void gsf::api_gsf::push_log(std::string msg)
{
	this->logs.emplace_back(std::move(msg));
}

void gsf::api_gsf::_api_log(std::string txt)
{
	this->push_log(std::move(txt));
}

bool gsf::api_gsf::_api_register_callback(std::string id, sol::function callback)
{
	auto hashed_id = utils::hash_fnv1a(id.c_str());
	api_gsf::callback *arr_callbacks = reinterpret_cast<api_gsf::callback *>(&this->callbacks);
	for (int i = 0; i < sizeof(this->callbacks) / sizeof(callback); ++i)
	{
		if (arr_callbacks[i].active == false && hashed_id == arr_callbacks[i].hashed_name)
		{
			arr_callbacks[i].reg(callback);
			return true;
		}
	}

	this->push_log("Invalid callback ID: " + id);
	return false;
}
