#include "hooks.h"
#include "../global.h"
#include <d3d11.h>
#include <MinHook.h>
#include <vector>

bool hooks::install()
{
	if (MH_Initialize() != MH_OK)
		return 0;

	// WindowProc
	hooks::ch_wndproc->init(global::game_window);

	// IDXGISwapChain::Present
	ID3D11Device   *dummy_device_ptr;
	IDXGISwapChain *dummy_swapchain_ptr;

	D3D_FEATURE_LEVEL fl;
    DXGI_SWAP_CHAIN_DESC scd =
    {
                                                     // DXGI_MODE_DESC
        {                                            
            800,                                     // Width
            600,                                     // Height
            { 60, 1 },                               // RefreshRate
            DXGI_FORMAT_R8G8B8A8_UNORM,              // Format
            DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,    // ScanlineOrdering
            DXGI_MODE_SCALING_UNSPECIFIED            // Scaling
        },                                           
                                                     // DXGI_SAMPLE_DESC
        {                                            
            1,                                       // Count
            0                                        // Quality
        },

        DXGI_USAGE_RENDER_TARGET_OUTPUT,             // BufferUsage
        1,                                           // BufferCount
        reinterpret_cast<HWND>(global::game_window), // OutputWindow
        TRUE,                                        // Windowed
        DXGI_SWAP_EFFECT_DISCARD,                    // SwapEffect
        0                                            // Flags
    };

    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_REFERENCE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &scd, &dummy_swapchain_ptr, &dummy_device_ptr, &fl, nullptr)))
    {
        return false;
    }
    
	hooks::ch_present->init(reinterpret_cast<void ***>(dummy_swapchain_ptr)[0][8]);
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
