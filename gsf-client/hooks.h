#pragma once

#include <hooking.h>

namespace gsf::hooks
{
	inline void *AnimatorIterator_get_speed_ptr = nullptr;

	bool install();
	bool uninstall();
}