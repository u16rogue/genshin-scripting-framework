#include "hooks.h"
#include "../global.h"
#include <d3d11.h>
#include <MinHook.h>
#include <vector>
#include <macro.h>
#include "../definitions.h"
#include "../game.h"

#pragma warning (disable: 26812)

// TODO: maybe implement dx init fallback incase finding the games dx fails
/*
static bool dx_init_fallback()
{
    ID3D11Device   *dummy_device_ptr;
	IDXGISwapChain *dummy_swapchain_ptr;

	D3D_FEATURE_LEVEL fl;
    DXGI_SWAP_CHAIN_DESC scd =
    {
        .BufferDesc
        {
            .Width            = 800,
            .Height           = 600,
            .RefreshRate      = { 60, 1 },
            .Format           = DXGI_FORMAT_R8G8B8A8_UNORM,
            .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
            .Scaling          = DXGI_MODE_SCALING_UNSPECIFIED
        },

        .SampleDesc
        {
            .Count   = 1,
            .Quality = 0
        },

        .BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount  = 1,
        .OutputWindow = reinterpret_cast<HWND>(global::game_window),
        .Windowed     = TRUE,
        .SwapEffect   = DXGI_SWAP_EFFECT_DISCARD,
        .Flags        = 0
    };

    if (!DEBUG_CON_C_LOG(L"Setup dummy @ D3D11CreateDeviceAndSwapChain...", SUCCEEDED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_REFERENCE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &scd, &dummy_swapchain_ptr, &dummy_device_ptr, &fl, nullptr))) || !dummy_swapchain_ptr)
    {
        return false;
    }

    ID3D11DeviceContext *dummy_device_context_ptr = nullptr;
    dummy_device_ptr->GetImmediateContext(&dummy_device_context_ptr);

    dummy_device_context_ptr->Release();
    dummy_swapchain_ptr->Release();
    dummy_device_ptr->Release();
}
*/

bool gsf::hooks::install()
{
    DEBUG_CON_PRINT("\nInstalling hooks...");

	if (!DEBUG_CON_C_LOG(L"Initializing MinHook...", MH_Initialize() == MH_STATUS::MH_OK))
		return 0;

	DEBUG_CON_C_LOG(L"WindowProcedure Callback", gsf::hooks::WindowProc.inhook(global::game_window));
    DEBUG_CON_C_LOG(L"ShowCursor",               gsf::hooks::ShowCursor.inhook(::ShowCursor));

    #define _INHOOK_UNITYENGINE(msg, api) DEBUG_CON_C_LOG(msg, gsf::hooks:: ## api ## .inhook(game:: ## api ## .get_ptr()))
    {
        _INHOOK_UNITYENGINE(L"UnityEngine.Cursor::set_lockState(UnityEngine.CursorLockMode)", UnityEngine_Cursor_set_lockState);
        _INHOOK_UNITYENGINE(L"UnityEngine.Cursor::get_visible()",                             UnityEngine_Cursor_set_visible);
        _INHOOK_UNITYENGINE(L"UnityEngine.JsonUtility::ToJson(System.Object,System.Boolean)", UnityEngine_JsonUtility_ToJson);
        _INHOOK_UNITYENGINE(L"UnityEngine.Utility::FromJson(System.String,System.Type)",      UnityEngine_JsonUtility_FromJson);
        _INHOOK_UNITYENGINE(L"UnityEngine.Input::GetButton(System.String)",                   UnityEngine_Input_GetButton);
        _INHOOK_UNITYENGINE(L"UnityEngine.Input::GetAxisRaw(System.String)",                  UnityEngine_Input_GetAxisRaw);
    }
    #undef _INHOOK_UNITYENGINE

    ID3D11DeviceContext *game_devicectx = game::get_dx_devicectx();
    DEBUG_CON_C_LOG(L"ID3D11DeviceContext::Draw", gsf::hooks::Draw.inhook(GET_VFUNC_FROM_VCLASS_BY_IDX(game_devicectx, 0, gsf::def::vtidx::ID3D11DeviceContext::Draw)));
    DEBUG_CON_C_LOG(L"ID3D11DeviceContext::DrawIndexed", gsf::hooks::DrawIndexed.inhook(GET_VFUNC_FROM_VCLASS_BY_IDX(game_devicectx, 0, gsf::def::vtidx::ID3D11DeviceContext::DrawIndexed)));
    DEBUG_CON_C_LOG(L"IDXGISwapChain::ResizeBuffers",    gsf::hooks::ResizeBuffers.inhook(GET_VFUNC_FROM_VCLASS_BY_IDX(global::dx_swapchain, 0, gsf::def::vtidx::IDXGISwapChain::ResizeBuffers)));
    DEBUG_CON_C_LOG(L"IDXGISwapChain::Present",          gsf::hooks::Present.inhook(GET_VFUNC_FROM_VCLASS_BY_IDX(global::dx_swapchain, 0, gsf::def::vtidx::IDXGISwapChain::Present)));

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