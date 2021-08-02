#include "hooks.h"
#include "../global.h"
#include <d3d11.h>
#include <MinHook.h>
#include <vector>
#include <macro.h>
#include "../definitions.h"

bool hooks::install()
{
    DEBUG_CON_PRINT("\nInstalling hooks...");

	if (!CON_C_LOG(L"Initializing MinHook...", MH_Initialize() == MH_OK))
		return 0;

	// WindowProc
	CON_C_LOG(L"Init WndProc hook...", hooks::ch_wndproc->init(global::game_window));

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



    if (!CON_C_LOG(L"D3D11CreateDeviceAndSwapChain...", SUCCEEDED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_REFERENCE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &scd, &dummy_swapchain_ptr, &dummy_device_ptr, &fl, nullptr))) || !dummy_swapchain_ptr)
    {
        return false;
    }

    ID3D11DeviceContext *dummy_device_context_ptr = nullptr;
    dummy_device_ptr->GetImmediateContext(&dummy_device_context_ptr);

	CON_C_LOG(L"Init IDXGISwapChain::Present hook...",   hooks::ch_present->init(GET_VFUNC_FROM_VCLASS_BY_IDX(dummy_swapchain_ptr, 0, gsf::def::vtidx::IDXGISwapChain::Present)));     // IDXGISwapChain::Present
    CON_C_LOG(L"Init ID3D11DeviceContext::Draw hook...", hooks::ch_draw->init(GET_VFUNC_FROM_VCLASS_BY_IDX(dummy_device_context_ptr, 0, gsf::def::vtidx::ID3D11DeviceContext::Draw))); // ID3D11DeviceContext::Draw

    dummy_device_context_ptr->Release();
	dummy_swapchain_ptr->Release();
	dummy_device_ptr->Release();

	// Hook all initialized hook instances
	for (auto &hook_instance : utils::hook_base::instances)
		if (!hook_instance->hook())
			return false;

	return true;
}

bool hooks::uninstall()
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
