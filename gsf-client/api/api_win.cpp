#include "api_win.h"
#include <winternal.h>

bool gsf::api_win::setup_api(sol::state &slua)
{
	auto namespace_win = slua["win"].get_or_create<sol::table>();
	namespace_win.set_function("find_module", &gsf::api_win::_api_find_module, this);
	namespace_win.set_function("get_all_modules", &gsf::api_win::_api_get_all_modules, this);

	return true;
}

sol::object gsf::api_win::_api_find_module(std::wstring module_name)
{
	auto ldr = utils::ldr_data_table_entry_find(module_name.c_str());

	if (ldr)
		return this->get_lua_state().create_table_with("base_address", reinterpret_cast<std::uintptr_t>(ldr->dll_base), "size", ldr->size_of_image);

	return sol::nil;
}

sol::table gsf::api_win::_api_get_all_modules()
{
	auto result = this->get_lua_state().create_table();

	utils::ldr_data_table_entry *entry = nullptr;
	while (utils::ldr_data_table_entry_next(entry))
	{
		if (entry->dll_base)
			result[entry->base_dll_name] = this->get_lua_state().create_table_with("base_address", reinterpret_cast<std::uintptr_t>(entry->dll_base), "size", entry->size_of_image);
	}

	return result;
}

