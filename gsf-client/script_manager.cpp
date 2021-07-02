#include "script_manager.h"

#define __IMPORT_SCRIPT_NAME "Import Script"

#include <Windows.h>
#include <imgui.h>
#include <filesystem>
#include <cstddef>
#include <cstdint>
#include <misc_utils.h>
#include <macro.h>
#include "helpers/imgui_prompts.h"

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

void import_script_draw()
{
    static char buffer_import[MAX_PATH] = { '\0' };
    
    ImGui::Text("Script Path:");
    ImGui::SameLine();
    bool input_enter_keyed = ImGui::InputText("##script_import_buffer", buffer_import, sizeof(buffer_import), ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    if (ImGui::Button("Import") || input_enter_keyed)
    {
        if (std::filesystem::exists(buffer_import))
        {
            script_instances.emplace_back(buffer_import);
            std::memset(buffer_import, '\0', sizeof(buffer_import));
            ImGui::CloseCurrentPopup();
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
}

helpers::imgui_popup_modal import_prompt = helpers::imgui_popup_modal(__IMPORT_SCRIPT_NAME, &import_script_draw);

#pragma endregion

void imported_script_list_draw()
{
    gsf::script *queued_for_removal = nullptr; // TODO: temp only. implement as mutex in the future

    for (gsf::script &inst : script_instances)
    {
        ImGui::Text("File:");
        ImGui::SameLine();

        static ImVec4 file_color_loaded   { 0.f, 1.f, 0.f, 1.f };
        static ImVec4 file_color_unloaded { 1.f, 0.f, 0.f, 1.f };

        ImGui::TextColored(inst ? file_color_loaded : file_color_unloaded, inst.get_filepath().data());
        
        #ifdef _DEBUG
        if (ImGui::Button("Remove"))
        {
            if (inst.unload())
                queued_for_removal = &inst;
        }
        #endif

        // show recent (and all) logs
        ImGui::SameLine();
        if (ImGui::Button("Show Logs"))
        {
            // TODO: pop up modal to show all logs
        }
        else if (ImGui::IsItemHovered())
        {
            auto &logs = inst.get_logs();
            ImGui::SetTooltip(logs.empty() ? "No recent logs" : logs.back().c_str());
        }

        ImGui::SameLine();
        if (ImGui::Button(inst ? "Unload" : "Load"))
        {
            if (!inst)
                inst.load();
            else
                inst.unload();
        }

        ImGui::Separator();
    }
}

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
                    import_prompt.show();

                #ifdef _DEBUG
                if (ImGui::MenuItem("Add Folder")) {}

                if (ImGui::MenuItem("Link")) {}
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
    
        imported_script_list_draw();
    }
    ImGui::End();
}