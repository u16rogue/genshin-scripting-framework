#include "hooks.h"
#include <d3d11.h>
#include "../global.h"

HRESULT __stdcall hk_ResizeBuffers(IDXGISwapChain *thisptr, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	// Cleanup
	if (global::dx_render_target_view)
	{
		global::dx_render_target_view->Release();
		global::dx_render_target_view = nullptr;
	}

	static auto o_fn = gsf::hooks::ResizeBuffers.get_original<decltype(hk_ResizeBuffers)>();
	auto result = o_fn(thisptr, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	// Create
	if (ID3D11Texture2D *dx_backbuffer = nullptr; SUCCEEDED(global::dx_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&dx_backbuffer))))
	{
		global::dx_device->CreateRenderTargetView(dx_backbuffer, nullptr, &global::dx_render_target_view);
		dx_backbuffer->Release();
	}

	return result;
}

utils::hook_detour gsf::hooks::ResizeBuffers(hk_ResizeBuffers);