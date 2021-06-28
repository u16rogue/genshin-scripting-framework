#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

namespace gsf::features::fps_counter
{
	bool &toggle();

	void on_present();
	void on_imgui_draw();
}