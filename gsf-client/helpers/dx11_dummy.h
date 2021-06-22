#pragma once

#include <d3d11.h>

namespace helpers
{
	bool dx11_dummy_device_dup(HWND target_window, ID3D11Device *&out_device_ptr, IDXGISwapChain *&out_swapchain_ptr);
}