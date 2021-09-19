#pragma once

#include "sdk/structures.h"

namespace game
{
	inline game::structs::player_map_coords *player_map_coords = nullptr;
	inline void *(__fastcall *get_object)(const char *name) = nullptr;

	bool init();
}