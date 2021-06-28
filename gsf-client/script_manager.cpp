#include "script_manager.h"

#include <Windows.h>
#include <imgui.h>
#include <filesystem>
#include <cstddef>
#include <cstdint>
#include <misc_utils.h>

//DEBUG
#include <macro.h>

char buffer_import[MAX_PATH] = { '\0' };
bool visible                 = false;

const char         *error_message         = "No error message provided.";
bool                error_message_visible = false;
utils::fader_float  error_message_fader   = utils::fader_float(1000, 3000);
void show_error(const char *msg)
{
    error_message = msg;
    error_message_fader.mark();
    error_message_visible = true;
}

bool &gsf::script_manager::get_visible_flag()
{
    return visible;
}

void gsf::script_manager::on_imgui_draw()
{
    if (!visible)
        return;
    
    static bool __setwindowsizeonce = []() -> bool { ImGui::SetNextWindowSize({ 595, 426 }); return true; } (); // uhhh
    if (ImGui::Begin("Script Manager", &visible, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text("Script Path:");
        ImGui::SameLine();
        ImGui::InputText("##script_import_buffer", buffer_import, sizeof(buffer_import));
        ImGui::SameLine();
        if (ImGui::Button("Import"))
        {
            if (std::filesystem::exists(buffer_import))
            {

            }
            else
            {
                show_error("File not found!");
            }
        }

        if (error_message_visible)
        {
            float errmsg_alpha_scale = error_message_fader.get();
            if (errmsg_alpha_scale > 0.f)
            {
                ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, ImVec4{ 1.f, 0.f, 0.f, errmsg_alpha_scale });
                ImGui::TextWrapped(error_message);
                ImGui::PopStyleColor();
            }
            else
            {
                error_message_visible = false;
            }
        }

        ImGui::Separator();

    }
    ImGui::End();
}