#include "game.h"

#include <macro.h>
#include <winternal.h>
#include <mem.h>
#include <misc_utils.h>
#include <hash.h>
#include "hooks.h"

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

	// Modules
	entryp_t mod_unity_player;
	entryp_t mod_user_assembly;

	// Load modules
	DEBUG_COUT("\n[>] LOAD MODULES:");
	if (!DEBUG_CON_C_LOG(L"UnityPlayer.dll",  hlp_load_module(mod_unity_player,  utils::hash_fnv1a_cv(L"UnityPlayer.dll")))
	||  !DEBUG_CON_C_LOG(L"UserAssembly.dll", hlp_load_module(mod_user_assembly, utils::hash_fnv1a_cv(L"UserAssembly.dll")))
	) {
		return false;
	}

	// Load exported functions
	DEBUG_COUT("\n[>] LOAD EXPORTS:");
	if (!DEBUG_CON_C_LOG(L"il2cpp_string_chars",  hlp_load_exportfn(mod_user_assembly, utils::hash_fnv1a_cv("il2cpp_string_chars"),  game::il2cpp_string_chars))
	||  !DEBUG_CON_C_LOG(L"il2cpp_string_length", hlp_load_exportfn(mod_user_assembly, utils::hash_fnv1a_cv("il2cpp_string_length"), game::il2cpp_string_length))
	||  !DEBUG_CON_C_LOG(L"il2cpp_string_new",    hlp_load_exportfn(mod_user_assembly, utils::hash_fnv1a_cv("il2cpp_string_new"),    game::il2cpp_string_new))
	) {
		return false;
	}

	// Signatures

	// Load signatures
	// TODO: switch to consteval ida signatures
	DEBUG_COUT("\n[>] LOAD SIGNATURES:");
	if (!DEBUG_CON_C_LOG(L"game::player_map_coords (ref)",                   hlp_aob_scan(mod_unity_player,  game::player_map_coords,                         "\xF2\x0F\x11\x0D\x00\x00\x00\x00\x48\x83\xC4\x00\x5B\xC3\x48\x8D\x0D",         "xxxx????xxx?xxxxx"))
	||  !DEBUG_CON_C_LOG(L"game::sdk::unity_scripting_api<>::get_unity_api", hlp_aob_scan(mod_user_assembly, game::sdk::unity_scripting_api<>::get_unity_api, "\x48\x8b\xc4\x48\x89\x48\x00\x55\x41\x54",                                     "xxxxxx?xxx"))
	||  !DEBUG_CON_C_LOG(L"game::dx_swapchain_ptr (ref)",                    hlp_aob_scan(mod_unity_player,  game::dx_swapchain_ptr,                          "\x48\x8B\x1D\x00\x00\x00\x00\x48\x8B\x8B\x00\x00\x00\x00\x48\x85\xC9\x74\x3A", "xxx????xxx????xxxxx"))
	||  !DEBUG_CON_C_LOG(L"game::dx_devicectx_ptr (ref)",                    hlp_aob_scan(mod_unity_player,  game::dx_devicectx_ptr,                          "\x4C\x8B\x3D\x00\x00\x00\x00\x4C\x8D",                                         "xxx????xx"))
	||  !DEBUG_CON_C_LOG(L"game::window_handle_ptr (ref)",                   hlp_aob_scan(mod_unity_player,  game::window_handle_ptr,                         "\x48\x89\x05\x00\x00\x00\x00\x48\x85\xC0\x0F\x84\x00\x00\x00\x00\xE8",         "xxx????xxxxx????x"))
	||  !DEBUG_CON_C_LOG(L"gsf::hooks::AnimatorIterator_get_speed_ptr",      hlp_aob_scan(mod_unity_player,  gsf::hooks::AnimatorIterator_get_speed_ptr,      "\x41\x83\xBF\x00\x00\x00\x00\x00\x74\x21",                                     "xxx?????xx"))
	) {
		return false;
	}

	// Load game functions
	DEBUG_COUT("\n[>] LOAD GAME FUNCTIONS:");
	if (!game::sdk::unity_scripting_api<>::load_function_all())
	{
		return false;
	}

	// Swapchain pointer
	struct
	{
		char pad0[0x3D0];
		union
		{
			void          **swapchain_vtable;
			IDXGISwapChain *swapchain_ptr;
		};
	} **_unk0 = reinterpret_cast<decltype(_unk0)>(utils::calc_rel2abs32(game::dx_swapchain_ptr, 0x7));

	while (!*_unk0 || !(*_unk0)->swapchain_ptr /* || !*(*_unk0)->swapchain_vtable */ )
		Sleep(1000);
	game::dx_swapchain_ptr = &(*_unk0)->swapchain_ptr;

	// Device context pointer
	game::dx_devicectx_ptr = reinterpret_cast<decltype(game::dx_devicectx_ptr)>(utils::calc_rel2abs32(game::dx_devicectx_ptr, 0x7));
	while (!*game::dx_devicectx_ptr)
		Sleep(1000);

	// player map coordinates
	game::player_map_coords = reinterpret_cast<decltype(game::player_map_coords)>(utils::calc_rel2abs32(game::player_map_coords, 0x8));

	// game window handle pointer
	game::window_handle_ptr = reinterpret_cast<decltype(game::window_handle_ptr)>(utils::calc_rel2abs32(game::window_handle_ptr, 0x7));

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	#if 1 // toggle

	// this is a band aid fix due to an issue when loading game values too early (especially when using gsf-launcher), the vfunc for the devicectx vtable does not have
	// the "right" pointers. the device and vtable address are the same regardless, its only the vfunc that differs and is causing the hooks to silently fail due to hooking
	// the wrong functions.
	// TODO: implement a better fix for invalid vfunc pointers in device context when loading too early...

	entryp_t mod_d3d11;
	void *Invalid_D3D_DrawIndexed = nullptr;
	void *Invalid_D3D_Draw = nullptr;

	DEBUG_COUT("\n[+] Appliying band-aid fix for invalid d3dctx vfuncs (see code for more details)...");
	if (!DEBUG_CON_C_LOG(L"d3d11.dll",                     hlp_load_module(mod_d3d11, utils::hash_fnv1a_cv(L"d3d11.dll")))
	||  !DEBUG_CON_C_LOG(L"Invalid_D3D_DrawIndexed (ref)", hlp_aob_scan(mod_d3d11, Invalid_D3D_DrawIndexed, "\xE8\x00\x00\x00\x00\xEB\x3F\xE8", "x????xxx"))
	||  ![&]() { Invalid_D3D_DrawIndexed = reinterpret_cast<decltype(Invalid_D3D_DrawIndexed)>(utils::calc_rel2abs32(Invalid_D3D_DrawIndexed, 0x5)); DEBUG_COUT(" -> 0x" << Invalid_D3D_DrawIndexed); return Invalid_D3D_DrawIndexed; } ()
	||  !DEBUG_CON_C_LOG(L"Invalid_D3D_Draw (ref)",        hlp_aob_scan(mod_d3d11, Invalid_D3D_Draw,        "\xE8\x00\x00\x00\x00\xEB\x3C\xE8", "x????xxx"))
	||  ![&]() { Invalid_D3D_Draw = reinterpret_cast<decltype(Invalid_D3D_Draw)>(utils::calc_rel2abs32(Invalid_D3D_Draw, 0x5)); DEBUG_COUT(" -> 0x" << Invalid_D3D_Draw); return Invalid_D3D_Draw; } ()
	) {
		return false;
	}

	void **devicectx_vtable = *reinterpret_cast<void ***>(*game::dx_devicectx_ptr);
	while (devicectx_vtable[12] == Invalid_D3D_DrawIndexed || devicectx_vtable[13] == Invalid_D3D_Draw)
		Sleep(1000);

	#endif

	return true;
	#pragma warning(default: 6011)
}