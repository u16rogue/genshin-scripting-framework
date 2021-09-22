#include "fps_counter.h"

#include <Windows.h>
#include <imgui.h>
#include <string>

static constexpr int total_delta = 1000; // 1000ms = 1s

std::size_t current_accumilation = 0;
std::size_t total_accumilation   = 0;
std::size_t last_accumilation    = 0;
std::size_t next_tick_accumilate = 0;

// std::string fps_str = "Accumilating...";

std::size_t gsf::features::fps_counter::get_fps()
{
	return total_accumilation;
}

// const char *gsf::features::fps_counter::get_fps_cstr()
// {
// 	return fps_str.c_str();
// }

void gsf::features::fps_counter::on_present()
{
	if (!active)
		return;

	auto current_tick = GetTickCount64();

	if (current_tick > next_tick_accumilate)
	{
		next_tick_accumilate = current_tick + total_delta;
		last_accumilation = total_accumilation;
		total_accumilation = current_accumilation;
		current_accumilation = 0;

		// if (total_accumilation != last_accumilation)
		// 	fps_str = std::to_string(total_accumilation);
	}

	++current_accumilation;
}
