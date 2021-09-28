#include "game.h"

#include <macro.h>
#include <winternal.h>
#include <mem.h>
#include <misc_utils.h>
#include <hash.h>

using entryp_t = utils::ldr_data_table_entry *;

template <typename T>
static bool hlp_aob_scan(entryp_t mod, T *&out_result, const char *sig, const char *mask)
{
	out_result = reinterpret_cast<T *>(utils::aob_scan(mod->dll_base, mod->size_of_image, sig, mask));
	if (!out_result)
		return false;

	DEBUG_COUT(" @ 0x" << reinterpret_cast<void *>(out_result));
	return true;
}

static bool hlp_load_module(entryp_t &out_result, const utils::fnv1a64_t hashed_name)
{
	out_result = utils::ldr_data_table_entry_find(hashed_name);
	if (!out_result)
		return false;

	DEBUG_COUT(" @ 0x" << reinterpret_cast<void *>(out_result->dll_base));
	return true;
}

template <typename T>
static bool hlp_load_exportfn(entryp_t mod, const utils::fnv1a64_t hashed_name, T *&out_result)
{
	out_result = reinterpret_cast<T *>(utils::export_fn_get(mod->dll_base, hashed_name));
	if (!out_result)
		return false;

	DEBUG_COUT(" @ 0x" << out_result);
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
	if (!DEBUG_CON_C_LOG(L"UnityPlayer.dll",  hlp_load_module(mod_unity_player,  utils::hash_fnv1a_cv(L"UnityPlayer.dll")))
	||  !DEBUG_CON_C_LOG(L"UserAssembly.dll", hlp_load_module(mod_user_assembly, utils::hash_fnv1a_cv(L"UserAssembly.dll")))
	) {
		return false;
	}

	// Load exported functions
	DEBUG_COUT("\nLOAD EXPORTS:");
	if (!DEBUG_CON_C_LOG(L"il2cpp_string_chars",  hlp_load_exportfn(mod_user_assembly, utils::hash_fnv1a_cv("il2cpp_string_chars"),  game::il2cpp_string_chars))
	||  !DEBUG_CON_C_LOG(L"il2cpp_string_length", hlp_load_exportfn(mod_user_assembly, utils::hash_fnv1a_cv("il2cpp_string_length"), game::il2cpp_string_length))
	||  !DEBUG_CON_C_LOG(L"il2cpp_string_new",    hlp_load_exportfn(mod_user_assembly, utils::hash_fnv1a_cv("il2cpp_string_new"),    game::il2cpp_string_new))
	) {
		return false;
	}

	// Load signatures
	DEBUG_COUT("\nLOAD SIGNATURES:");
	if (!DEBUG_CON_C_LOG(L"game::player_map_coords (ref)",                     hlp_aob_scan(mod_unity_player,  sig_player_map_coord,                              "\xF2\x0F\x11\x0D\x00\x00\x00\x00\x48\x83\xC4\x00\x5B\xC3\x48\x8D\x0D",  "xxxx????xxx?xxxxx"))
	||  !DEBUG_CON_C_LOG(L"game::sdk::unity_scripting_api<>::get_api_by_name", hlp_aob_scan(mod_user_assembly, game::sdk::unity_scripting_api<>::get_api_by_name, "\x48\x8b\xc4\x48\x89\x48\x00\x55\x41\x54",                              "xxxxxx?xxx"))
	) {
		return false;
	}

	// Load game functions
	DEBUG_COUT("\nLOAD GAME FUNCTIONS:");
	if (!game::sdk::unity_scripting_api<>::load_function_all())
	{
		return false;
	}

	game::player_map_coords = reinterpret_cast<game::sdk::player_map_coords *>(utils::calc_rel2abs32(sig_player_map_coord, 0x8));

	return true;
	#pragma warning(default: 6011)
}