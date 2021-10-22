#include "hooks.h"
#include "../global.h"
#include <d3d11.h>
#include <MinHook.h>
#include <vector>
#include <macro.h>
#include "../game.h"
#include <intrin.h>

#pragma warning (disable: 26812)

enum class Indexes_IDXGISwapChain
{
	Present = 8,
	ResizeBuffers = 13
};

enum class Indexes_ID3D11DeviceContext
{
	DrawIndexed = 12,
	Draw = 13
};

bool gsf::hooks::install()
{
    DEBUG_CON_PRINT("\nInstalling hooks...");

	if (!DEBUG_CON_C_LOG(L"Initializing MinHook...", MH_Initialize() == MH_STATUS::MH_OK))
		return false;

    ID3D11DeviceContext *game_devicectx = game::get_dx_devicectx();
    #define _INHOOK_UNITYENGINE(msg, api) DEBUG_CON_C_LOG(msg, gsf::hooks:: ## api ## .inhook(game:: ## api ## .get_ptr()))

    if (!DEBUG_CON_C_LOG(L"WindowProcedure Callback",         gsf::hooks::WindowProc                        .inhook(*game::window_handle_ptr))
    ||  !DEBUG_CON_C_LOG(L"ShowCursor",                       gsf::hooks::ShowCursor                        .inhook(::ShowCursor))
    ||  !DEBUG_CON_C_LOG(L"ID3D11DeviceContext::Draw",        gsf::hooks::Draw                              .inhook(GET_VFUNC_FROM_VCLASS_BY_IDX(game_devicectx,       0, Indexes_ID3D11DeviceContext::Draw)))
    ||  !DEBUG_CON_C_LOG(L"ID3D11DeviceContext::DrawIndexed", gsf::hooks::DrawIndexed                       .inhook(GET_VFUNC_FROM_VCLASS_BY_IDX(game_devicectx,       0, Indexes_ID3D11DeviceContext::DrawIndexed)))
    ||  !DEBUG_CON_C_LOG(L"IDXGISwapChain::ResizeBuffers",    gsf::hooks::ResizeBuffers                     .inhook(GET_VFUNC_FROM_VCLASS_BY_IDX(global::dx_swapchain, 0, Indexes_IDXGISwapChain::ResizeBuffers)))
    ||  !DEBUG_CON_C_LOG(L"IDXGISwapChain::Present",          gsf::hooks::Present                           .inhook(GET_VFUNC_FROM_VCLASS_BY_IDX(global::dx_swapchain, 0, Indexes_IDXGISwapChain::Present)))
    ||  !DEBUG_CON_C_LOG(L"UnityEngine_Cursor_set_lockState", gsf::hooks::UnityEngine_Cursor_set_lockState  .inhook(game::UnityEngine_Cursor_set_lockState.get_ptr()))
    ||  !DEBUG_CON_C_LOG(L"UnityEngine_Cursor_set_visible",   gsf::hooks::UnityEngine_Cursor_set_visible    .inhook(game::UnityEngine_Cursor_set_visible.get_ptr()))
    ||  !DEBUG_CON_C_LOG(L"UnityEngine_JsonUtility_ToJson",   gsf::hooks::UnityEngine_JsonUtility_ToJson    .inhook(game::UnityEngine_JsonUtility_ToJson.get_ptr()))
    ||  !DEBUG_CON_C_LOG(L"UnityEngine_JsonUtility_FromJson", gsf::hooks::UnityEngine_JsonUtility_FromJson  .inhook(game::UnityEngine_JsonUtility_FromJson.get_ptr()))
    ||  !DEBUG_CON_C_LOG(L"UnityEngine_Input_GetButton",      gsf::hooks::UnityEngine_Input_GetButton       .inhook(game::UnityEngine_Input_GetButton.get_ptr()))
    ||  !DEBUG_CON_C_LOG(L"UnityEngine_Input_GetAxisRaw",     gsf::hooks::UnityEngine_Input_GetAxisRaw      .inhook(game::UnityEngine_Input_GetAxisRaw.get_ptr()))
    ||  !DEBUG_CON_C_LOG(L"UnityEngine_Animator_get_speed",   gsf::hooks::UnityEngine_Animator_get_speed    .inhook(game::sdk::Animator::_get_speed.get_ptr()))
    ) {
        return false;
    }

    #undef _INHOOK_UNITYENGINE

    return true;
}

bool gsf::hooks::uninstall()
{
	// Copy the entire list to a vector before iterating through and calling unhook.
	// This is done since we can't iterate the list while modifying it in the middle of an iteration.
	// Modifying it in terms of calling unhook which calls internal_on_unhook which then removes itself from the static instances std::list
	std::vector<utils::hook_base *> hook_instances { std::begin(utils::hook_base::instances), std::end(utils::hook_base::instances) };

	for (auto &hook_instance : hook_instances)
		if (!hook_instance->unhook())
			return false;

	return true;
}

#pragma warning (default: 26812)