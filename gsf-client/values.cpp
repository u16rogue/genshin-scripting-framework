#include "values.h"

#include <macro.h>
#include <winternal.h>
#include <pattern_scan.h>
#include <misc_utils.h>

// TODO: clean up and refactor

bool gsf::values::load()
{
	#pragma warning(disable: 6011)
	DEBUG_COUT("Starting to load values...\nLoading modules...");

	// -- Module loading

	auto mod_unity_player = utils::ldr_data_table_entry_find(L"UnityPlayer.dll");
	if (!CON_C_LOG(L"Load from LDR: UserAssembly.dll", mod_unity_player))
		return false;

	// -- End of module loading

	// Player map coordinates
	{
		auto sig_player_map_coord = utils::aob_scan(mod_unity_player->dll_base, mod_unity_player->size_of_image, "\xF2\x0F\x11\x0D\x00\x00\x00\x00\x48\x83\xC4\x00\x5B\xC3\x48\x8D\x0D", "xxxx????xxx?xxxxx");
		if (!CON_C_LOG(L"Signature: Player map coordinate", sig_player_map_coord))
			return false;

		gsf::values::player_map_coords = reinterpret_cast<gsf::sdk::player_map_coords *>(utils::calc_rel_address_32(sig_player_map_coord, 0x8, 0x4));
		DEBUG_COUT("... 0x" << gsf::values::player_map_coords);
	}


	return true;
	#pragma warning(default: 6011)
}