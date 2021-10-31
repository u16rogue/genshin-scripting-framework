#pragma once

#include "sdk/sdk.h"

namespace game
{
	inline IDXGISwapChain      **dx_swapchain_ptr = nullptr;
	inline ID3D11DeviceContext **dx_devicectx_ptr = nullptr;

	inline HWND                         *window_handle_ptr = nullptr;
	inline game::sdk::player_map_coords *player_map_coords = nullptr;

	inline game::sdk::il2cpp_string_chars_t  il2cpp_string_chars  = nullptr;
	inline game::sdk::il2cpp_string_length_t il2cpp_string_length = nullptr;
	inline game::sdk::il2cpp_string_new_t    il2cpp_string_new    = nullptr;

	bool init();
}