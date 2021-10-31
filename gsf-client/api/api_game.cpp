#include "api_game.h"
#include "../game.h"

bool gsf::api_game::setup_api(sol::state &slua)
{
	auto namespace_game = slua["game"].get_or_create<sol::table>();
	namespace_game.set_function("get_map_coords", &gsf::api_game::_api_get_map_coords, this);
	namespace_game.set_function("get_fn", [](const char *name) { return reinterpret_cast<std::uintptr_t>(game::sdk::unity_scripting_api<>::get_unity_api(name)); });
	namespace_game.set_function("object_get_name", [](std::uintptr_t object) { return game::il2cpp_string_chars(reinterpret_cast<game::sdk::Object *>(object)->get_name()); });

	return true;
}

sol::table gsf::api_game::_api_get_map_coords()
{
	return this->get_lua_state().create_table_with("x", game::player_map_coords->x, "z", game::player_map_coords->z);
}