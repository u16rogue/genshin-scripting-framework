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

void helpers::imgui_popup_modal::on_imgui_draw()
{
	for (helpers::imgui_popup_modal *&instance : helpers::imgui_popup_modal::instances)
	{
		if (instance->prompt_requested)
		{
			ImGui::OpenPopup(instance->str.c_str());
			instance->prompt_requested = false;
		}

		if (ImGui::BeginPopupModal(instance->str.c_str(), nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove))
		{
			instance->callback();
			ImGui::EndPopup();
		}
	}
}
