#include "dx11_dummy.h"

#include <dxgi.h>
#include <macro.h>

bool helpers::dx11_dummy_device_dup(HWND target_window, ID3D11Device *&out_device_ptr, IDXGISwapChain *&out_swapchain_ptr)
{
    D3D_FEATURE_LEVEL fl;
    DXGI_SWAP_CHAIN_DESC scd =
    {
                                                  // DXGI_MODE_DESC
        {
            800,                                  // Width
            600,                                  // Height
            { 60, 1 },                            // RefreshRate
            DXGI_FORMAT_R8G8B8A8_UNORM,           // Format
            DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, // ScanlineOrdering
            DXGI_MODE_SCALING_UNSPECIFIED         // Scaling
        },
                                                  // DXGI_SAMPLE_DESC
        {
            1,                                    // Count
            0                                     // Quality
        },

        DXGI_USAGE_RENDER_TARGET_OUTPUT,          // BufferUsage
        1,                                        // BufferCount
        target_window,                            // OutputWindow
        TRUE,                                     // Windowed
        DXGI_SWAP_EFFECT_DISCARD,                 // SwapEffect
        0                                         // Flags
    };

    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_REFERENCE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &scd, &out_swapchain_ptr, &out_device_ptr, &fl, nullptr)))
    {
        DEBUG_WCOUT("\nCreate device failed error: " << GetLastError());
        return false;
    }

    DEBUG_WCOUT("\nCreate dummy device successful!");
	return true;
}