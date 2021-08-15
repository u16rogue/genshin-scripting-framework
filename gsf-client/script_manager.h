#pragma once

#include <vector>
#include <utility>
#include "script.h"

namespace gsf::script_manager
{
	inline bool visible = false;
	void on_imgui_draw();
	const std::vector<gsf::script> &get_scripts();
	bool import_script(std::string_view file_path);
}