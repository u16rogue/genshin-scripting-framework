#pragma once

#include <d3d11.h>

namespace global
{
	inline void *dll_handle = nullptr;
	inline bool cursor_is_visible = true; // TODO: decide if its still necessary alongside the ShowCursor hook

	inline ID3D11Device           *dx_device             = nullptr;
	inline IDXGISwapChain         *dx_swapchain          = nullptr;
	inline ID3D11DeviceContext    *dx_context            = nullptr;
	inline ID3D11RenderTargetView *dx_render_target_view = nullptr;
}