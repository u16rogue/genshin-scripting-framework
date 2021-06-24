#include "hooks.h"
#include "../global.h"
#include "../helpers/dx11_dummy.h"
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
	if (!helpers::dx11_dummy_device_dup(reinterpret_cast<HWND>(global::game_window), dummy_device_ptr, dummy_swapchain_ptr))
		return false;
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
