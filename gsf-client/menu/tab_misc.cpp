#include "tab_misc.h"
#include <imgui.h>
#include "../features/fps_counter.h"
#include "../menu/menu.h"

void gsf::menu::tab_misc::render_tab()
{
	if (ImGui::BeginTabItem("Misc"))
	{
		ImGui::Text("Information window: ");
		ImGui::Checkbox("Display FPS", &gsf::features::fps_counter::active);

		ImGui::EndTabItem();
	}
}

void gsf::menu::tab_misc::render_window()
{
	if (!gsf::features::fps_counter::active)
		return;

	ImGuiWindowFlags w_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (!gsf::menu::is_open)
		w_flags |= ImGuiWindowFlags_NoInputs;

	ImGui::SetNextWindowPos({ 4.f, 4.f }, ImGuiCond_Once);
	if (ImGui::Begin("##info_window", nullptr, w_flags))
	{
		if (gsf::features::fps_counter::active)
			ImGui::Text("FPS: %d", gsf::features::fps_counter::get_fps());
	}
	ImGui::End();
}
