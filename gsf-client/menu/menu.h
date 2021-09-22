#pragma once

#include <Windows.h>

namespace gsf::menu
{
	inline bool is_open = false;

	bool windowproc(UINT msg, WPARAM wParam, LPARAM lParam);
	void render_imgui();
}