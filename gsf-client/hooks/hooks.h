#pragma once

#include <hooking.h>

namespace gsf::hooks
{
	extern std::unique_ptr<utils::hook_detour>  ch_present;
	extern std::unique_ptr<utils::hook_wndproc> ch_wndproc;
	extern std::unique_ptr<utils::hook_detour>  ch_draw;
	extern std::unique_ptr<utils::hook_detour>  ch_drawindexed;

	bool install();
	bool uninstall();
}