#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

namespace gsf::features::fps_counter
{
	inline bool active = true;

	std::size_t get_fps();
	// const char *get_fps_cstr();
	void on_present();
}