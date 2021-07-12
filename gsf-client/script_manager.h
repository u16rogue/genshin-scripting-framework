#pragma once

#include <vector>
#include <utility>
#include "script.h"

namespace gsf::script_manager
{
	bool &get_visible_flag();
	void on_imgui_draw();
	const std::vector<gsf::script> &get_scripts();
}