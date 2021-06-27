#include "gsf_client.h"
#include "global.h"

#include <Windows.h>
#include <imgui.h>

#include <misc_utils.h>
#include <console.h>
#include <macro.h>

#include "git_hash.h"
#include "definitions.h"

#include "hooks/hooks.h"
#include "features/fps_counter.h"

bool gsf::init()
{
    if ((global::game_window = FindWindowW(UTILS_A2W_MDEF(GSF_DEF_GAME_WND_CLASS), UTILS_A2W_MDEF(GSF_DEF_GAME_WND_TITLE))) == nullptr
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

    if (con::is_allocated())
        FreeConsole();

    if (HANDLE exit_thread = nullptr; exit_thread = CreateThread(nullptr, NULL, [](LPVOID arg0) -> DWORD
    {
        FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(global::dll_handle), 0);
        return 0;
    }, nullptr, NULL, nullptr)) { CloseHandle(exit_thread); }

	return true;
}

static char script_directory_buffer[MAX_PATH] = { '\0' };
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
        if (ImGui::Button("Import"))
        {

        }
    }
    ImGui::End();
}

static bool gsf_about_menu_visible = true;
void gsf_about_menu_render()
{
    if (ImGui::Begin("About", &gsf_about_menu_visible, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize))
    {
        ImGui::Text(
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
    ImGui::End();
}

void gsf::render_imgui()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(
            "Genshin Scripting Framework"
            #if defined(_DEBUG) && !defined(_NDEBUG)
            " [DEBUG]"
            #endif
        ))
        {

            if (ImGui::MenuItem("Script Manager"))
                gsf_script_manager_menu_visible = !gsf_script_manager_menu_visible;

            ImGui::Checkbox("FPS Counter", &gsf::features::fps_counter::si_get().active);

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

            if (ImGui::MenuItem("About"))
                gsf_about_menu_visible = !gsf_about_menu_visible;

            ImGui::Separator();

            if (ImGui::MenuItem("Shutdown (Delete key)"))
                gsf::shutdown();

            ImGui::EndMenu();
        }

        ImGui::Separator();

        gsf::features::fps_counter::si_get().on_imgui_draw();
    }
    ImGui::EndMainMenuBar();

    if (gsf_script_manager_menu_visible)
        gsf_script_manager_menu_render();

    if (gsf_about_menu_visible)
        gsf_about_menu_render();
}