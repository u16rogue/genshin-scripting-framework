#include "gsf_client.h"
#include "global.h"

#include <Windows.h>
#include <imgui.h>

#include <misc_utils.h>
#include <console.h>
#include <macro.h>

#include "git_info.h"
#include "definitions.h"

#include "script_manager.h"
#include "helpers/imgui_prompts.h"
#include "hooks/hooks.h"
#include "game.h"
#include "features/fps_counter.h"
#include "menu/menu.h"

#if __has_include("autoexecdef.h") && !defined( GSF_AUTOEXEC_SCRIPT_PATH )
    #include "autoexecdef.h"
#endif

static bool get_game_window_handle(void *&handle_out)
{
    // int timeout = 30;

    while (handle_out == nullptr /* && --timeout > 0 */)
    {
        handle_out = FindWindowW(UTILS_A2W_MDEF(GSF_DEF_GAME_WND_CLASS), UTILS_A2W_MDEF(GSF_DEF_GAME_WND_TITLE));
        Sleep(1000);
    }

    return handle_out != nullptr;
}

bool gsf::init()
{
    #ifdef _DEBUG
    {
        if (!con::init() || !SetConsoleTitleW(utils::random_str().c_str()))
            return false;
    }
    #endif

    #ifdef GSF_AUTOEXEC_SCRIPT_PATH
    {
        if (!gsf::script_manager::script_autoexec(GSF_AUTOEXEC_SCRIPT_PATH))
            return false;
    }
    #endif

    if (CURSORINFO ci = { .cbSize = sizeof(ci) }; DEBUG_CON_C_LOG(L"Loading cursor info", GetCursorInfo(&ci)))
        global::cursor_is_visible = ci.flags == CURSOR_SHOWING;

    ImGui::CreateContext();
    if (!DEBUG_CON_C_LOG(L"Loading game window handle", get_game_window_handle(global::game_window)) || !game::init() || !gsf::hooks::install())
        return false;

    ImGui::GetIO().IniFilename = nullptr;

	return true;
}

bool gsf::shutdown()
{
    if (HANDLE exit_thread = nullptr; exit_thread = CreateThread(nullptr, NULL, [](LPVOID arg0) -> DWORD
    {
        hooks::uninstall();

        if (con::is_allocated())
            FreeConsole();

        HWND con_wnd = reinterpret_cast<HWND>(con::get_window());
        if (con_wnd)
            PostMessageW(con_wnd, WM_CLOSE, NULL, NULL);

        FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(global::dll_handle), 0);
        return 0;
    }, nullptr, NULL, nullptr)) { CloseHandle(exit_thread); }

	return true;
}

static void gsf_draw_dropmenu()
{

}

static void gsf_draw_menubaritems()
{
    gsf::features::fps_counter::on_gsf_draw_menubaritems();
}

void gsf::render_imgui()
{
    auto &imported_scripts = gsf::script_manager::get_scripts();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(
            "Genshin Scripting Framework"
            #if defined(_DEBUG) && !defined(_NDEBUG)
            " [DEBUG]"
            #endif
        ))
        {
            gsf_draw_dropmenu();
            ImGui::EndMenu();
        }

        ImGui::Separator();
        ImGui::Text("(%d/%d)", gsf::script::count_loaded_scripts, imported_scripts.size());
        ImGui::Separator();
        gsf_draw_menubaritems();
    }
    ImGui::EndMainMenuBar();

    gsf::menu::render_imgui();

    for (auto &script : imported_scripts)
    {
        auto &callback = script->get_callbacks().on_imgui_draw;

        if (!callback.active)
            continue;

        const std::lock_guard<std::mutex> lock(callback.mutex);

        switch (script->get_current_state())
        {
            case gsf::script::state::UNLOADED:
            case gsf::script::state::UNLOADING:
                continue;
        }

        callback.callback_function();

        while (script->imgui_active_begin_count)
        {
            ImGui::End();
            --script->imgui_active_begin_count;
        }
    }

    helpers::imgui_popup_modal::on_imgui_draw();
}