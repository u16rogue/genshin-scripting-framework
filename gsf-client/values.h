#pragma once

#include <cstdint>
#include "sdk/structures.h"

namespace gsf::values
{
	inline gsf::sdk::player_map_coords *player_map_coords = nullptr;

	bool load();
}