#include "api_imgui.h"
#include <imgui.h>

bool gsf::api_imgui::setup_api(sol::state &slua)
{
	auto namespace_imgui = slua["imgui"].get_or_create<sol::table>();

	namespace_imgui.set_function("begin", &gsf::api_imgui::_api_begin, this);
	namespace_imgui.set_function("iend", &gsf::api_imgui::_api_iend, this);
	namespace_imgui.set_function("check_box", &gsf::api_imgui::_api_check_box, this);
	namespace_imgui.set_function("input_text", &gsf::api_imgui::_api_input_text, this);

	namespace_imgui.set_function("text", [](const char *text) { ImGui::Text(text); });
	namespace_imgui.set_function("same_line", []() { ImGui::SameLine(); });
	namespace_imgui.set_function("button", [](const char *text) -> bool { return ImGui::Button(text); });
	namespace_imgui.set_function("separator", &ImGui::Separator);
	namespace_imgui.set_function("push_id", static_cast<void(*)(int)>(&ImGui::PushID));
	namespace_imgui.set_function("pop_id", &ImGui::PopID);

	namespace_imgui.set_function("draw_text", [](const char *text, float x, float y, float r, float g, float b, float a)
	{
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(x, y), ImColor(r, g, b, a), text);
	});

	namespace_imgui.set_function("draw_line", [](float x1, float y1, float x2, float y2, float thickness, float r, float g, float b, float a)
	{
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImColor(r, g, b, a), thickness);
	});

	namespace_imgui.set_function("draw_rect", [](float x1, float y1, float x2, float y2, bool filled, float thickness, float r, float g, float b, float a)
	{
		if (filled)
		{
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImColor(r, g, b, a));
		}
		else
		{
			ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), ImColor(r, g, b, a), 0.f, 15, thickness);
		}
	});

	return true;
}

bool gsf::api_imgui::_api_begin(const char *text)
{
	++api_imgui::imgui_active_begin_count;
	return ImGui::Begin(text, nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);
}

void gsf::api_imgui::_api_iend()
{
	--api_imgui::imgui_active_begin_count;
	return ImGui::End();
}

bool gsf::api_imgui::_api_check_box(const char *label)
{
	auto &container = *reinterpret_cast<bool *>(&this->imgui_data_ref[label]);
	ImGui::Checkbox(label, &container);
	return container;
}

const char *gsf::api_imgui::_api_input_text(const char *label)
{
	auto &container = this->imgui_data_ref[label];
	ImGui::InputText(label, container, sizeof(container));
	return container;
}