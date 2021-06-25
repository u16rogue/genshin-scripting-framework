#pragma once

#include <d3d11.h>

namespace helpers
{
	/// <summary>
	/// Creates a DirectX 11 instance for the purpose of creating a dummy device and obtaining the shared pointers of DX11.
	/// </summary>
	/// <param name="target_window">- Window handle of where the main DX11 is rendering at.</param>
	/// <param name="out_device_ptr">- [OUT] Destination for the resulting ID3D11Device pointer.</param>
	/// <param name="out_swapchain_ptr">- [OUT] Destination for the resulting IDXGISwapChain pointer.</param>
	/// <returns>Returns true if the dummy device was successfuly created, otherwise returns false.</returns>
	bool dx11_dummy_device_dup(HWND target_window, ID3D11Device *&out_device_ptr, IDXGISwapChain *&out_swapchain_ptr);
}