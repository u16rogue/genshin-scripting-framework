#include "script_manager.h"

#include <Windows.h>
#include <imgui.h>

char buffer_import[MAX_PATH] = { '\0' };
bool visible = false;

bool &gsf::script_manager::toggle()
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

        }
    }
    ImGui::End();
}