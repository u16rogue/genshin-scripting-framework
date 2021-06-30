#include "imgui_prompts.h"

helpers::imgui_popup_modal::imgui_popup_modal(std::string_view str_, callback_t callback_)
	: str(str_), callback(callback_)
{
	helpers::imgui_popup_modal::instances.push_back(this);
}

void helpers::imgui_popup_modal::show()
{
	this->prompt_requested = true;
}

void helpers::imgui_popup_modal::__on_imgui_draw()
{
	if (this->prompt_requested)
	{
		ImGui::OpenPopup(this->str.c_str());
		this->prompt_requested = false;
	}

	if (ImGui::BeginPopupModal(this->str.c_str(), nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove))
	{
		this->callback();
		ImGui::EndPopup();
	}
}
