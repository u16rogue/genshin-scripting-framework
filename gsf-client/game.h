#pragma once

#include "sdk/sdk.h"

namespace game
{
	inline game::sdk::player_map_coords *player_map_coords = nullptr;
	inline game::sdk::get_fn_t get_fn = nullptr;
	inline game::sdk::UnityEngine_Cursor_set_visible_t engine_set_cursor_visible = nullptr;
	inline game::sdk::UnityEngine_Cursor_set_lockState_t engine_set_cursor_lockstate = nullptr;

	bool init();
}