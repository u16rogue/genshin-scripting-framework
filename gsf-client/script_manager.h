#pragma once

#include <vector>
#include <utility>
#include "script.h"

namespace gsf::script_manager
{
	inline bool visible = false;
	void on_imgui_draw();
	const std::vector<gsf::script> &get_scripts();

	bool script_import(std::string_view file_path, gsf::script **script_instance_out = nullptr);
}