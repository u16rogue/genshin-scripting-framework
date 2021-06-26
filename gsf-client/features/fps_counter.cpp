#include "fps_counter.h"

#include <Windows.h>
#include <imgui.h>
#include <string>

void gsf::features::fps_counter::on_present()
{
	if (!this->active)
		return;

	auto current_tick = GetTickCount64();

	if (current_tick > this->next_tick_accumilate)
	{
		this->next_tick_accumilate = current_tick + fps_counter::total_delta;
		this->last_accumilation = this->total_accumilation;
		this->total_accumilation = this->current_accumilation;
		this->current_accumilation = 0;
	}

	++this->current_accumilation;
}

void gsf::features::fps_counter::on_imgui_draw()
{
	if (!this->active)
		return;

	if (this->total_accumilation != this->last_accumilation)
		this->fps_str = "FPS: " + std::to_string(this->total_accumilation);

	ImGui::Text(this->fps_str.c_str());
}
