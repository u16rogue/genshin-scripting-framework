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
	while (!(out_result = utils::ldr_data_table_entry_find(hashed_name)))
		Sleep(1000);

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
	if (!DEBUG_CON_C_LOG(L"game::player_map_coords (ref)",                   hlp_aob_scan(mod_unity_player,  game::player_map_coords,                           "\xF2\x0F\x11\x0D\x00\x00\x00\x00\x48\x83\xC4\x00\x5B\xC3\x48\x8D\x0D",                     "xxxx????xxx?xxxxx"))
	||  !DEBUG_CON_C_LOG(L"game::sdk::unity_scripting_api<>::get_unity_api", hlp_aob_scan(mod_user_assembly, game::sdk::unity_scripting_api<>::get_unity_api, "\x48\x8b\xc4\x48\x89\x48\x00\x55\x41\x54",                                                 "xxxxxx?xxx"))
	||  !DEBUG_CON_C_LOG(L"game::dx_swapchain_ptr (ref)",                    hlp_aob_scan(mod_unity_player,  game::dx_swapchain_ptr,                            "\xe8\x00\x00\x00\x00\x4c\x8b\xf0\x48\x85\xc0\x74\x00\x48\x89\x5c\x24\x00\x48\x89\x6c\x24", "x????xxxxxxx?xxxx?xxxx"))
	||  !DEBUG_CON_C_LOG(L"game::dx_devicectx_ptr (ref)",                    hlp_aob_scan(mod_unity_player,  game::dx_devicectx_ptr,                            "\xe8\x00\x00\x00\x00\x44\x8b\x8c\x24\x00\x00\x00\x00\x48\x8d\x8b",                         "x????xxxx????xxx"))
	// ||  !DEBUG_CON_C_LOG(L"game::window_handle_ptr (ref)",                     hlp_aob_scan(mod_unity_player,  game::window_handle_ptr,                           "\x48\x89\x05\x00\x00\x00\x00\x48\x85\xc0\x0f\x84\x00\x00\x00\x00\xe8",                     "x????????xxx"))
	) {
		return false;
	}

	// Load game functions
	DEBUG_COUT("\nLOAD GAME FUNCTIONS:");
	if (!game::sdk::unity_scripting_api<>::load_function_all())
	{
		return false;
	}

	// Swapchain pointer
	auto get_dx_swapchain_prologue = utils::calc_rel2abs32(game::dx_swapchain_ptr, 0x5);
	std::uint8_t *swapchain_base  = nullptr;
	DEBUG_COUT("\nWaiting for swapchain_base pointer...");
	while ((swapchain_base = *reinterpret_cast<std::uint8_t **>(utils::calc_rel2abs32(get_dx_swapchain_prologue, 0x7))) == nullptr)
		Sleep(800);
	std::int32_t swapchain_offset = *reinterpret_cast<std::int32_t *>(reinterpret_cast<std::uintptr_t>(get_dx_swapchain_prologue) + 0xA);
	game::dx_swapchain_ptr = reinterpret_cast<decltype(game::dx_swapchain_ptr)>(swapchain_base + swapchain_offset);

	// device context
	game::dx_devicectx_ptr  = reinterpret_cast<decltype(game::dx_devicectx_ptr)>(utils::calc_rel2abs32(utils::calc_rel2abs32(game::dx_devicectx_ptr, 0x5), 0x7));

	game::player_map_coords = reinterpret_cast<decltype(game::player_map_coords)>(utils::calc_rel2abs32(game::player_map_coords, 0x8));

	// TODO: dynamically resolve window handle pointer
	game::window_handle_ptr = reinterpret_cast<decltype(game::window_handle_ptr)>(mod_unity_player->dll_base + 0x1AB7BA8);// reinterpret_cast<decltype(game::window_handle_ptr)>(utils::calc_rel2abs32(game::window_handle_ptr, 0x7));

	return true;
	#pragma warning(default: 6011)
}