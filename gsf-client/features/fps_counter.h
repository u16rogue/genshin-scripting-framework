#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

namespace gsf::features::fps_counter
{
	bool &get_active_flag();

	void on_present();
	void on_gsf_draw_menubaritems();
}