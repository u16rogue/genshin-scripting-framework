#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

namespace gsf::features::fps_counter
{
	inline bool active = true;

	void on_present();
	void on_gsf_draw_menubaritems();
}