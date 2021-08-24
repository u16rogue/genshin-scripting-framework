#pragma once

namespace gsf::sdk
{
	// UnityPlayer.dll - f2 0f 11 0d ? ? ? ? 48 83 c4 ? 5b c3 48 8d 0d
	// Used by the game to calculate distance for waypoints
	struct player_map_coords
	{
		float x;
		float z;
	};
}