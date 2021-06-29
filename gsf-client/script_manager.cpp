#include "script_manager.h"

#define __IMPORT_SCRIPT_NAME "Import Script"

#include <Windows.h>
#include <imgui.h>
#include <filesystem>
#include <cstddef>
#include <cstdint>
#include <misc_utils.h>
#include <macro.h>

std::vector<gsf::script> script_instances;
bool                     visible          = false;

bool &gsf::script_manager::get_visible_flag()
{
    return visible;
}

const std::vector<gsf::script> &gsf::script_manager::get_scripts()
{
    return script_instances;
}

#pragma region Script Import

const char         *error_message         = "No error message provided.";
bool                error_message_visible = false;
utils::fader_float  error_message_fader   = utils::fader_float(1000, 3000);
void show_error(const char *msg)
{
    error_message = msg;
    error_message_fader.mark();
    error_message_visible = true;
}

enum class import_mode_e
{
    UNKNOWN,
    FILE,
    FOLDER,
    LINK
};

bool import_prompt_req_open = false;
import_mode_e import_mode = import_mode_e::UNKNOWN;

void import_script_prompt(import_mode_e mode_ = import_mode_e::UNKNOWN)
{
    if (mode_ == import_mode_e::UNKNOWN)
        return;
    
    import_mode = mode_;
    import_prompt_req_open = true;
}

void import_script_draw()
{
    if (import_prompt_req_open)
    {
        ImGui::OpenPopup(__IMPORT_SCRIPT_NAME);
        import_prompt_req_open = false;
    }
    
    if (ImGui::BeginPopupModal(__IMPORT_SCRIPT_NAME, nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char buffer_import[MAX_PATH] = { '\0' };

        ImGui::Text("Script Path:");
        ImGui::SameLine();
        ImGui::InputText("##script_import_buffer", buffer_import, sizeof(buffer_import));
        ImGui::SameLine();
        if (ImGui::Button("Import"))
        {
            if (std::filesystem::exists(buffer_import))
            {
                script_instances.emplace_back(buffer_import);
            }
            else
            {
                show_error("File not found!");
            }
        }
        
        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();

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

        ImGui::EndPopup();
    }
}

#pragma endregion

void gsf::script_manager::on_imgui_draw()
{
    if (!visible)
        return;
    
    ImGui::SetNextWindowSize({ 595, 426 }, ImGuiCond_::ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Script Manager", &visible, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Import"))
            {
                if (ImGui::MenuItem("Add File"))
                    import_script_prompt(import_mode_e::FILE);

                #ifdef _DEBUG
                if (ImGui::MenuItem("Add Folder"))
                    import_script_prompt(import_mode_e::FOLDER);

                if (ImGui::MenuItem("Link"))
                    import_script_prompt(import_mode_e::LINK);
                #endif

                ImGui::EndMenu();
            }

            #ifdef _DEBUG
            if (ImGui::BeginMenu("Permissions"))
            {

                ImGui::EndMenu();
            }
            #endif

            ImGui::EndMenuBar();
        }

        for (gsf::script &inst : script_instances)
            ImGui::Text(inst.get_filepath().data());

        import_script_draw();
    }
    ImGui::End();
}