#include "gsf_client.h"

#include <Windows.h>
#include <imgui.h>
#include "git_hash.h"
#include "hooks/hooks.h"
#include "global.h"
#include <misc_utils.h>
#include <console.h>

bool gsf::init()
{
    if ((global::game_window = FindWindowW(L"UnityWndClass", L"Genshin Impact")) == nullptr
    #ifdef _DEBUG
    || !con::init()
    || !SetConsoleTitleW(utils::random_str().c_str())
    #endif
    || !hooks::install()
    ) {
        return false;
    }

    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;

	return true;
}

bool gsf::shutdown()
{
    hooks::uninstall();

    if (HANDLE exit_thread = nullptr; exit_thread = CreateThread(nullptr, NULL, [](LPVOID arg0) -> DWORD
    {
        FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(global::dll_handle), 0);
        return 0;
    }, nullptr, NULL, nullptr)) { CloseHandle(exit_thread); }

	return true;
}

char script_directory_buffer[MAX_PATH] = { '\0' };
static bool gsf_script_manager_menu_visible = false;
void gsf_script_manager_menu_render()
{
    static bool __setwindowsizeonce = []() -> bool { ImGui::SetNextWindowSize({ 595, 426 }); return true; } (); // uhhh
    if (ImGui::Begin("Script Manager", &gsf_script_manager_menu_visible, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text("Script Directory:");
        ImGui::SameLine();
        ImGui::InputText("##script_directory_buffer", script_directory_buffer, sizeof(script_directory_buffer));
        ImGui::SameLine();
        ImGui::Button("Search");
    }
    ImGui::End();
}

void gsf::render_imgui()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Genshin Scripting Framework"))
        {

            if (ImGui::BeginMenu("Theme"))
            {
                if (ImGui::MenuItem("Default"))
                    ImGui::StyleColorsClassic();

                if (ImGui::MenuItem("Light"))
                    ImGui::StyleColorsLight();

                if (ImGui::MenuItem("Dark"))
                    ImGui::StyleColorsDark();

                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Shutdown (Delete)"))
                gsf::shutdown();

            ImGui::EndMenu();
        }
        else if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "Scripting API and Framework for Genshin Impact\n"
                "\n"

                "Build type: "
                #if defined(_DEBUG) && !defined(NDEBUG)
                "Debug"
                #elif defined(NDEBUG) && !defined(_DEBUG)
                "Release"
                #else
                "Unknown"
                #endif
                "\n"

                "Build Date: " __DATE__ " " __TIME__ "\n"
                "Git hash built @ " GSF_CLIENT_GIT_HASH "\n"
                "\n"
                "https://github.com/rogueeeee/genshin-scripting-framework\n"
                "\n"
                "This software is licensed under the GNU General Public License 3.0\n"
            );
        }
        ImGui::Separator();

        if (ImGui::MenuItem("Script Manager"))
            gsf_script_manager_menu_visible = !gsf_script_manager_menu_visible;
    }
    ImGui::EndMainMenuBar();

    if (gsf_script_manager_menu_visible)
        gsf_script_manager_menu_render();
}