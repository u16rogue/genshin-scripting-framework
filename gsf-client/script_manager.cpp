#include "script_manager.h"

#include <Windows.h>
#include <imgui.h>
#include <filesystem>
#include <cstddef>
#include <cstdint>
#include <misc_utils.h>
#include <macro.h>
#include <thread>
#include "helpers/imgui_prompts.h"

// TODO: optional header to load autoexec lua thing

std::vector<gsf::script> script_instances;
bool                     visible = false;

bool         script_log_window_visible  = false;
gsf::script *script_log_window_selected = nullptr; // Points to the script instance to read the log from for the log window

const char         *error_message         = "No error message provided.";
bool                error_message_visible = false;
utils::fader_float  error_message_fader   = utils::fader_float(1000, 3000);

bool &gsf::script_manager::get_visible_flag()
{
    return visible;
}

const std::vector<gsf::script> &gsf::script_manager::get_scripts()
{
    return script_instances;
}

void show_error(const char *msg)
{
    error_message = msg;
    error_message_fader.mark();
    error_message_visible = true;
}

void import_prompt_callback()
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
            bool already_exists = false;
            for (auto &inst : script_instances)
            {
                if (inst.get_filepath() == buffer_import)
                {
                    already_exists = true;
                    break;
                }
            }

            if (already_exists)
            {
                show_error("Script already imported!");
            }
            else
            {
                script_instances.emplace_back(buffer_import);
                std::memset(buffer_import, '\0', sizeof(buffer_import));
                ImGui::CloseCurrentPopup();
            }
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

helpers::imgui_popup_modal import_prompt = helpers::imgui_popup_modal("Import Script", &import_prompt_callback);

void imported_scripts_list_draw()
{
    for (gsf::script &inst : script_instances)
    {
        ImGui::PushID(&inst);

        ImGui::Text("File:");
        ImGui::SameLine();

        static ImVec4 file_color_loaded   { 0.f, 1.f, 0.f, 1.f };
        static ImVec4 file_color_unloaded { 1.f, 0.f, 0.f, 1.f };

        ImGui::TextColored(inst ? file_color_loaded : file_color_unloaded, inst.get_filepath().data());

        /*
        #ifdef _DEBUG
        if (ImGui::Button("Remove"))
        {
            if (inst.unload())
            {
                // queued_for_removal = &inst;
            }
        }
        #endif
        */

        if (ImGui::Button("Show Logs"))
        {
            if (script_log_window_selected == &inst || !script_log_window_selected)
                script_log_window_visible = !script_log_window_visible;

            script_log_window_selected = &inst;
        }
        else if (ImGui::IsItemHovered())
        {
            auto &logs = inst.get_logs();
            ImGui::SetTooltip(logs.empty() ? "No recent logs" : logs.back().c_str());
        }

        ImGui::SameLine();
        if (ImGui::Button("Options"))
        {

        }

        ImGui::SameLine();
        auto script_state = inst.get_current_state();

        if (script_state == gsf::script::state::LOADING)
        {
            ImGui::Text("Loading...");
        }
        else if (ImGui::Button(script_state == gsf::script::state::LOADED ? "Unload" : "Load"))
        {
            // uhhh...
            std::thread([](auto script, auto state)
            {
                if (state == gsf::script::state::LOADED)
                {
                    script->unload();
                }
                else
                {
                    script->load();
                }
            }, &inst, script_state).detach();
        }

        ImGui::Separator();
        ImGui::PopID();
    }
}

void script_log_window_draw()
{
    if (!script_log_window_visible)
        return;

    ImGui::SetNextWindowSize({ 446, 336 }, ImGuiCond_::ImGuiCond_FirstUseEver);
    // TODO: use window title to display current log file
    if (ImGui::Begin("Script Logs ###script_logs_window", &script_log_window_visible, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse))
    {
        const char *header_text = "No script selected.";
        if (script_log_window_selected)
            header_text = script_log_window_selected->get_filepath().data();

        ImGui::Text("Logs of script file: %s", header_text);

        if (script_log_window_selected)
        {
            ImGui::BeginChild("Script Logs List", ImVec2(0, 0), true);

            const auto &curr_logs = script_log_window_selected->get_logs();
            for (long long i = curr_logs.size() - 1; i >= 0; --i) //for (auto log_entry = curr_logs.rbegin(); log_entry != curr_logs.rend(); ++log_entry)
            {
                ImGui::TextWrapped("[%d] %s", i, curr_logs[i].c_str());
                // ImGui::Separator();
            }

            ImGui::EndChild();
        }
    }
    ImGui::End();
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

        imported_scripts_list_draw();
    }
    ImGui::End();

    script_log_window_draw();
}