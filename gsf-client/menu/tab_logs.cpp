#include "tab_logs.h"
#include <imgui.h>
#include "../log_manager.h"
#include "menu.h"

static bool logs_floating      = false;
static int  logs_display_count = 0;

static ImVec4 log_type_colors[gsf::log_manager::log_type::_SIZE] =
{
	{ 101.f / 255.f, 251.f / 255.f, 136.f / 255.f, 1.f },
	{ 239.f / 255.f, 136.f / 255.f, 251.f / 255.f, 1.f },
	{ 136.f / 255.f, 212.f / 255.f, 251.f / 255.f, 1.f },
	{ 247.f / 255.f, 251.f / 255.f, 136.f / 255.f, 1.f },
	{ 1.f, 1.f, 1.f, 1.f }
};

static void render_logs_contents()
{
	const auto &logs = gsf::log_manager::get_logs();
	ImGui::BeginChild("##logs_list", ImVec2(0, 0), true);
	auto log_size = logs.size() - 1;
	for (auto i = log_size; i != -1 && (logs_display_count <= 0 || (log_size - i) < logs_display_count); --i)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, log_type_colors[logs[i].type]);
		ImGui::TextWrapped(logs[i].txt.c_str());
		ImGui::PopStyleColor();
	}
	ImGui::EndChild();
}

void gsf::menu::tab_logs::render_tab()
{
	if (ImGui::BeginTabItem("Logs"))
	{
		ImGui::Text("Log colors:");
		ImGui::SameLine();
		ImGui::ColorEdit4("Game", reinterpret_cast<float *>(&log_type_colors[gsf::log_manager::log_type::GAME]), ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();
		ImGui::ColorEdit4("Scripts", reinterpret_cast<float *>(&log_type_colors[gsf::log_manager::log_type::SCRIPTS]), ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();
		ImGui::ColorEdit4("Lua", reinterpret_cast<float *>(&log_type_colors[gsf::log_manager::log_type::LUA]), ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();
		ImGui::ColorEdit4("GSF", reinterpret_cast<float *>(&log_type_colors[gsf::log_manager::log_type::GSF]), ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();
		ImGui::ColorEdit4("Unspecified", reinterpret_cast<float *>(&log_type_colors[gsf::log_manager::log_type::UNSPEC]), ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();
		if (ImGui::Button(logs_floating ? "Dock Tab" : "Float Window")) logs_floating = !logs_floating;
		ImGui::SameLine();
		if (ImGui::Button("Clear logs")) gsf::log_manager::clear_log();
		ImGui::SameLine();
		ImGui::Text("Display count");
		ImGui::SameLine();
		ImGui::InputInt2("##disc", &logs_display_count);

		if (!logs_floating)
			render_logs_contents();

		ImGui::EndTabItem();
	}
}

void gsf::menu::tab_logs::render_window()
{
	if (!logs_floating)
		return;

	ImGui::SetNextWindowSize({ 300, 200 }, ImGuiCond_::ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Logs", &logs_floating, gsf::menu::is_open ? 0 : (ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_::ImGuiWindowFlags_NoInputs)))
	{
		render_logs_contents();
	}
	ImGui::End();
}
