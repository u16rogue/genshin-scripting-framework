#include "game.h"

#include <macro.h>
#include <winternal.h>
#include <mem.h>
#include <misc_utils.h>

using entryp_t = utils::ldr_data_table_entry *;

static bool w_aob_scan(entryp_t mod, void **out_result, const char *sig, const char *mask)
{
	void *&out = *out_result;
	out = utils::aob_scan(mod->dll_base, mod->size_of_image, sig, mask);
	if (!out)
		return false;

	DEBUG_COUT(" @ 0x" << out);
	return true;
}

static bool w_load_module(entryp_t &out_result, const wchar_t *name)
{
	out_result = utils::ldr_data_table_entry_find(name);
	if (!out_result)
		return false;

	DEBUG_COUT(" @ 0x" << reinterpret_cast<void *>(out_result->dll_base));
	return true;
}

static bool w_load_gamefn(const char *name, void **out_result)
{
	void *&out = *out_result;
	out = game::get_fn(name);
	if (!out)
		return false;

	DEBUG_COUT(" @ 0x" << out);
	return true;
}

bool game::init()
{
	#pragma warning(disable: 6011)
	DEBUG_COUT("\nStarting to load values...");

	// Modules
	entryp_t mod_unity_player;
	entryp_t mod_user_assembly;

	// Signature Results
	void *sig_player_map_coord;

	// Load modules
	DEBUG_COUT("\nLOAD MODULES:");
	if (!DEBUG_CON_C_LOG(L"UnityPlayer.dll",  w_load_module(mod_unity_player,  L"UnityPlayer.dll"))
	||  !DEBUG_CON_C_LOG(L"UserAssembly.dll", w_load_module(mod_user_assembly, L"UserAssembly.dll"))
	) {
		return false;
	}

	// Load signatures
	DEBUG_COUT("\nLOAD SIGNATURES:");
	if (!DEBUG_CON_C_LOG(L"game::player_map_coords (ref)", w_aob_scan(mod_unity_player, &sig_player_map_coord, "\xF2\x0F\x11\x0D\x00\x00\x00\x00\x48\x83\xC4\x00\x5B\xC3\x48\x8D\x0D", "xxxx????xxx?xxxxx"))
	||  !DEBUG_CON_C_LOG(L"game::get_fn", w_aob_scan(mod_user_assembly, reinterpret_cast<void **>(&game::get_fn), "\x48\x8b\xc4\x48\x89\x48\x00\x55\x41\x54", "xxxxxx?xxx"))
	) {
		return false;
	}

	// Load game functions
	DEBUG_COUT("\nLOAD GAME FUNCTIONS:");
	if (!DEBUG_CON_C_LOG(L"UnityEngine.Cursor::set_visible(System.Boolean)", w_load_gamefn("UnityEngine.Cursor::set_visible(System.Boolean)", reinterpret_cast<void **>(&game::engine_set_cursor_visible)))
	||  !DEBUG_CON_C_LOG(L"UnityEngine.Cursor::set_lockState(UnityEngine.CursorLockMode)", w_load_gamefn("UnityEngine.Cursor::set_lockState(UnityEngine.CursorLockMode)", reinterpret_cast<void **>(&game::engine_set_cursor_lockstate)))
	) {
		return false;
	}

	game::player_map_coords = reinterpret_cast<game::sdk::player_map_coords *>(utils::calc_rel_address_32(sig_player_map_coord, 0x8));

	return true;
	#pragma warning(default: 6011)
}