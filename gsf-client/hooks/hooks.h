#pragma once

#include <hooking.h>

namespace hooks
{	
	extern std::unique_ptr<utils::hook_detour> ch_present;
	extern std::unique_ptr<utils::hook_wndproc> ch_wndproc;
}