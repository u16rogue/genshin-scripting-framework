#include "fps_counter.h"

#include <Windows.h>
#include <imgui.h>
#include <string>

static constexpr int total_delta = 1000; // 1000ms = 1s

std::size_t current_accumilation = 0;
std::size_t total_accumilation   = 0;
std::size_t last_accumilation    = 0;
std::size_t next_tick_accumilate = 0;

std::string fps_str = "FPS: Accumilating...";

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
	}

	++current_accumilation;
}

void gsf::features::fps_counter::on_gsf_draw_menubaritems()
{
	if (!active)
		return;

	if (total_accumilation != last_accumilation)
		fps_str = "FPS: " + std::to_string(total_accumilation);

	ImGui::Text(fps_str.c_str());
}
