#include "tab_misc.h"
#include <imgui.h>
#include <Windows.h>
#include "../menu/menu.h"
#include "../callback_manager.h"

static bool toggle_fps_counter = true;

void gsf::menu::tab_misc::render_tab()
{
	if (ImGui::BeginTabItem("Misc"))
	{
		ImGui::Checkbox("Lock on_imgui_draw callback thread with mutex", &gsf::callback_manager::use_mut_on_imgui_draw);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Disabling this might make things faster but might cause instability.");

		ImGui::Text("Information window: ");
		ImGui::Checkbox("Display FPS", &toggle_fps_counter);

		ImGui::EndTabItem();
	}
}

void gsf::menu::tab_misc::render_window()
{
	if (!toggle_fps_counter)
		return;

	ImGuiWindowFlags w_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (!gsf::menu::is_open)
		w_flags |= ImGuiWindowFlags_NoInputs;

	ImGui::SetNextWindowPos({ 4.f, 4.f }, ImGuiCond_Once);
	if (ImGui::Begin("##info_window", nullptr, w_flags))
	{
		if (static auto &imgui_io = ImGui::GetIO(); toggle_fps_counter)
			ImGui::Text("FPS: %.1f", imgui_io.Framerate);
	}
	ImGui::End();
}
