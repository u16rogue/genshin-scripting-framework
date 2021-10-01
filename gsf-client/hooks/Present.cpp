#include "hooks.h"
#include "../global.h"
#include <Windows.h>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include <macro.h>
#include "../menu/menu.h"
#include "../script_manager.h"
#include "../helpers/imgui_prompts.h"

HRESULT __stdcall hk_Present(IDXGISwapChain *thisptr, UINT SyncInterval, UINT Flags)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    gsf::menu::render_imgui();

    for (auto &script : gsf::script_manager::get_scripts())
    {
        auto &callback = script->get_callbacks().on_imgui_draw;

        if (!callback.active)
            continue;

        bool should_mutex = script->get_config().imgui_mutex;

        if (should_mutex)
            callback.mutex.lock();

        switch (script->get_current_state())
        {
            case gsf::script::state::UNLOADED:
            case gsf::script::state::UNLOADING:
            {
                if (should_mutex)
                    callback.mutex.unlock();
                continue;
            }
        }

        callback.callback_function();

        while (script->imgui_active_begin_count)
        {
            ImGui::End();
            --script->imgui_active_begin_count;
        }

        if (should_mutex)
            callback.mutex.unlock();
    }

    helpers::imgui_popup_modal::on_imgui_draw();

    ImGui::Render();

    global::dx_context->OMSetRenderTargets(1, &global::dx_render_target_view, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    static auto o_Present = gsf::hooks::Present.get_original<decltype(hk_Present)>();
    auto result = o_Present(thisptr, SyncInterval, Flags);

    return result;
}

utils::hook_detour gsf::hooks::Present(hk_Present);