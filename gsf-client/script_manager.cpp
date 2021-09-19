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

static std::vector<std::unique_ptr<gsf::script>> script_instances;

static bool         script_log_window_visible  = false;
static gsf::script *script_log_window_selected = nullptr; // Points to the script instance to read the log from for the log window
static const char * const script_log_window_id = "###script_logs_window";
static std::string  script_log_window_title    = std::string("Script Logs (Nothing selected)") + script_log_window_id;

static const char         *error_message         = "No error message provided.";
static bool                error_message_visible = false;
static utils::fader_float  error_message_fader   = utils::fader_float(1000, 3000);

const std::vector<std::unique_ptr<gsf::script>> &gsf::script_manager::get_scripts()
{
    return script_instances;
}

bool gsf::script_manager::script_autoexec(std::string_view file_path)
{
    if (!std::filesystem::exists(file_path))
        return false;

    sol::state autoexec_script;

    auto namespace_gsf = autoexec_script["gsf"].get_or_create<sol::table>();
    namespace_gsf.set_function("import", [](const char *file_path) -> bool
    {
        return gsf::script_manager::script_import(file_path);
    });

    namespace_gsf.set_function("load", [](const char *file_path) -> bool
    {
        if (gsf::script *script = nullptr; gsf::script_manager::script_import(file_path, &script))
            return script->load();

        return false;
    });

    autoexec_script.script_file(file_path.data());

    return true;
}

bool gsf::script_manager::script_import(std::string_view file_path, gsf::script **script_instance_out)
{
    if (!std::filesystem::exists(file_path))
        return false;

    for (auto &script : script_instances)
        if (script->get_filepath() == file_path)
            return false;

    auto &loaded_script = script_instances.emplace_back(std::make_unique<gsf::script>(file_path));
    if (script_instance_out)
        *script_instance_out = loaded_script.get();

    return true;
}

void import_prompt_ui()
{
    static char buffer_import[MAX_PATH] = { '\0' };

    ImGui::Text("Script Path:");
    ImGui::SameLine();
    bool input_enter_keyed = ImGui::InputText("##script_import_buffer", buffer_import, sizeof(buffer_import), ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    if (ImGui::Button("Import") || input_enter_keyed)
    {
        if (gsf::script_manager::script_import(buffer_import))
        {
            std::memset(buffer_import, '\0', sizeof(buffer_import));
            ImGui::CloseCurrentPopup();
        }
        else
        {
            error_message = "Failed to import script!";
            error_message_fader.mark();
            error_message_visible = true;
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

static helpers::imgui_popup_modal import_prompt = helpers::imgui_popup_modal("Import Script", &import_prompt_ui);

void draw_imported_script_items()
{
    for (auto &inst : script_instances)
    {
        ImGui::PushID(&inst);

        ImGui::Text("File: %s", inst->get_filename().data());

        if (ImGui::Button("Show Logs"))
        {
            // NOTE: no issues right now since scripts cant be removed, pointer should be nulled back in the future
            if (script_log_window_selected == inst.get() || !script_log_window_selected)
                script_log_window_visible = !script_log_window_visible;

            script_log_window_selected = inst.get();
            script_log_window_title = std::string("Script Logs (") + script_log_window_selected->get_filename().data() + ")" + script_log_window_id;
        }
        else if (ImGui::IsItemHovered())
        {
            auto &logs = inst->get_logs();
            ImGui::SetTooltip(logs.empty() ? "No recent logs" : logs.back().c_str());
        }

        ImGui::SameLine();
        if (ImGui::Button("Options"))
        {

        }

        ImGui::SameLine();
        auto script_state = inst->get_current_state();

        if (script_state == gsf::script::state::LOADING || script_state == gsf::script::state::UNLOADING)
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
            }, inst.get(), script_state).detach();
        }

        ImGui::Separator();
        ImGui::PopID();
    }
}

void script_log_window_draw()
{
    if (!script_log_window_visible || !script_log_window_selected)
        return;

    ImGui::SetNextWindowSize({ 446, 336 }, ImGuiCond_::ImGuiCond_FirstUseEver);
    if (ImGui::Begin(script_log_window_title.c_str(), &script_log_window_visible, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse) && script_log_window_selected)
    {
        ImGui::BeginChild("Script Logs List", ImVec2(0, 0), true);

        const auto &curr_logs = script_log_window_selected->get_logs();
        for (auto i = curr_logs.size() - 1; i != -1; --i) //for (auto log_entry = curr_logs.rbegin(); log_entry != curr_logs.rend(); ++log_entry)
        {
            ImGui::TextWrapped("[%d] %s", i, curr_logs[i].c_str());
            // ImGui::Separator();
        }

        ImGui::EndChild();
    }
    ImGui::End();
}

void gsf::script_manager::on_imgui_draw()
{
    script_log_window_draw();

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

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        draw_imported_script_items();
    }
    ImGui::End();
}