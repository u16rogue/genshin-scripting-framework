#include "hooks.h"
#include "global.h"
#include <d3d11.h>
#include <MinHook.h>
#include <vector>
#include <macro.h>
#include "game.h"
#include <intrin.h>
#include "script_manager.h"
#include "callback_manager.h"
#include <Windows.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include "menu/menu.h"
#include "helpers/imgui_prompts.h"
#include "gsf_client.h"
#include <mem.h>
#include <winapi_helper.h>

#pragma warning (disable: 26812)

enum class Indexes_IDXGISwapChain
{
	Present = 8,
	ResizeBuffers = 13
};

enum class Indexes_ID3D11DeviceContext
{
	DrawIndexed = 12,
	Draw = 13
};

namespace gsf::hooks
{
    extern utils::hook_detour  Present;
    extern utils::hook_detour  ResizeBuffers;
    extern utils::hook_wndproc WindowProc;
    extern utils::hook_detour  Draw;
    extern utils::hook_detour  DrawIndexed;
    extern utils::hook_detour  ShowCursor;
    extern utils::hook_detour  UnityEngine_Cursor_set_lockState;
    extern utils::hook_detour  UnityEngine_Cursor_set_visible;
    extern utils::hook_detour  UnityEngine_JsonUtility_ToJson;
    extern utils::hook_detour  UnityEngine_JsonUtility_FromJson;
    extern utils::hook_detour  UnityEngine_Input_GetButton;
    extern utils::hook_detour  UnityEngine_Input_GetAxisRaw;
    extern utils::hook_detour  UnityEngine_Animator_get_speed;
}
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void __stdcall hk_Draw(ID3D11DeviceContext *thisptr, UINT VertexCount, UINT StartVertexLocation)
{
    if (gsf::callback_manager::get_callbacks().dx_draw.dispatch_cancelable(VertexCount, StartVertexLocation))
        return;

    static auto o_Draw = gsf::hooks::Draw.get_original<decltype(hk_Draw)>();
    return o_Draw(thisptr, VertexCount, StartVertexLocation);
}

utils::hook_detour gsf::hooks::Draw(hk_Draw);

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void __stdcall hk_DrawIndexed(ID3D11DeviceContext *thisptr, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
    if (gsf::callback_manager::get_callbacks().dx_drawindexed.dispatch_cancelable(IndexCount, StartIndexLocation, BaseVertexLocation))
        return;

    static auto o_DrawIndexed = gsf::hooks::DrawIndexed.get_original<decltype(hk_DrawIndexed)>();
    return o_DrawIndexed(thisptr, IndexCount, StartIndexLocation, BaseVertexLocation);
}

utils::hook_detour gsf::hooks::DrawIndexed(hk_DrawIndexed);

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

HRESULT __stdcall hk_Present(IDXGISwapChain *thisptr, UINT SyncInterval, UINT Flags)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    gsf::menu::render_imgui();

    if (gsf::callback_manager::use_mut_on_imgui_draw)
    {
        const std::lock_guard lg(gsf::callback_manager::mut_on_imgui_draw);
        gsf::callback_manager::get_callbacks().on_imgui_draw.dispatch();
    }
    else
    {
        gsf::callback_manager::get_callbacks().on_imgui_draw.dispatch();
    }

    // unwind begin calls
    while (gsf::api_imgui::imgui_active_begin_count)
    {
        ImGui::End();
        --gsf::api_imgui::imgui_active_begin_count;
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

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

HRESULT __stdcall hk_ResizeBuffers(IDXGISwapChain *thisptr, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    // Cleanup
    if (global::dx_render_target_view)
    {
        global::dx_render_target_view->Release();
        global::dx_render_target_view = nullptr;
    }

    static auto o_fn = gsf::hooks::ResizeBuffers.get_original<decltype(hk_ResizeBuffers)>();
    auto result = o_fn(thisptr, BufferCount, Width, Height, NewFormat, SwapChainFlags);

    // Create
    if (ID3D11Texture2D *dx_backbuffer = nullptr; SUCCEEDED(global::dx_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&dx_backbuffer))))
    {
        global::dx_device->CreateRenderTargetView(dx_backbuffer, nullptr, &global::dx_render_target_view);
        dx_backbuffer->Release();
    }

    return result;
}

utils::hook_detour gsf::hooks::ResizeBuffers(hk_ResizeBuffers);

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int __stdcall hk_ShowCursor(BOOL bShow)
{
    if (gsf::menu::is_open && !bShow)
        return 0;

    static auto o_ShowCursor = gsf::hooks::ShowCursor.get_original<decltype(hk_ShowCursor)>();
    auto result = o_ShowCursor(bShow);
    global::cursor_is_visible = result != -1;

    return result;
}

utils::hook_detour gsf::hooks::ShowCursor(hk_ShowCursor);

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void __fastcall WorldEntityIterator_get_speed_shim(game::sdk::AnimatorStateInfo_MAYBE *AnimatorState)
{
    if (!AnimatorState->unk0) // cmp     dword ptr [r15+0ECh], 0
        return;

    auto rax = AnimatorState->unk1; // mov     rax, [r15+4A0h]
    if (!rax)                       // test    rax, rax
        return;

    auto rcx = rax->unk0; // mov     rcx, [rax+0F0h]
    if (!rcx)             // test    rcx, rcx
        return;

    float result = AnimatorState->speed; // mov     eax, [r15+314h]

    gsf::callback_manager::get_callbacks().on_animator_speed.dispatch_returnable(result, reinterpret_cast<std::uintptr_t>(AnimatorState->parent), result);

    *rcx = result; // mov     [rcx], eax
}

constexpr auto WorldEntityIterator_get_speed_codeinj_hotzone_size = 43;

std::uint8_t WorldEntityIterator_get_speed_codeinj[] =
{
    0x4C, 0x89, 0xF9,                                           // mov rcx, r15 (rcx is volatile in this context)
    0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, <WorldEntityIterator_get_speed_shim>
    0xFF, 0xD0,                                                 // call rax
    0xE9, 0x00, 0x00, 0x00, 0x00                                // jmp < WorldEntityIterator_get_speed_codeinj_hotzone_size - sizeof(WorldEntityIterator_get_speed_codeinj) + 1>
};

std::uint8_t WorldEntityIterator_get_speed_original[sizeof(WorldEntityIterator_get_speed_codeinj)] {};

float hk_UnityEngine_Animator_get_speed(game::sdk::Animator *animatorObj)
{
    float result = animatorObj->state_info->speed;
    gsf::callback_manager::get_callbacks().on_animator_speed.dispatch_returnable(result, reinterpret_cast<std::uintptr_t>(animatorObj), result);
    return result;
}

utils::hook_detour gsf::hooks::UnityEngine_Animator_get_speed(hk_UnityEngine_Animator_get_speed);

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void hk_UnityEngine_Cursor_set_lockState(game::sdk::CursorLockMode mode)
{
    if (gsf::menu::is_open && mode == game::sdk::CursorLockMode::Locked)
        return;

    static auto o_fn = gsf::hooks::UnityEngine_Cursor_set_lockState.get_original<decltype(hk_UnityEngine_Cursor_set_lockState)>();
    return o_fn(mode);
}

utils::hook_detour gsf::hooks::UnityEngine_Cursor_set_lockState(hk_UnityEngine_Cursor_set_lockState);

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void hk_UnityEngine_Cursor_set_visible(game::sdk::Boolean visible)
{
    if (gsf::menu::is_open && !visible)
        return;

    static auto o_fn = gsf::hooks::UnityEngine_Cursor_set_visible.get_original<decltype(hk_UnityEngine_Cursor_set_visible)>();
    return o_fn(visible);
}

utils::hook_detour gsf::hooks::UnityEngine_Cursor_set_visible(hk_UnityEngine_Cursor_set_visible);

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

float hk_UnityEngine_Input_GetAxisRaw(game::sdk::Il2CppString *axisName)
{
    auto hashed_axis_name = utils::hash_fnv1a(game::il2cpp_string_chars(axisName));
    for (auto &force_axis : gsf::api_controls::flags_look)
    {
        if (force_axis.game_identifier != hashed_axis_name)
            continue;

        if (force_axis.amount == 0.f)
            break;

        float force_val = force_axis.amount;
        force_axis.amount = 0.f;

        return force_val;
    }

    static auto o_fn = gsf::hooks::UnityEngine_Input_GetAxisRaw.get_original<decltype(hk_UnityEngine_Input_GetAxisRaw)>();
    return o_fn(axisName);
}

utils::hook_detour gsf::hooks::UnityEngine_Input_GetAxisRaw(hk_UnityEngine_Input_GetAxisRaw);

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

game::sdk::Boolean hk_UnityEngine_Input_GetButton(game::sdk::Il2CppString *buttonName)
{
    auto hashed_btn_name = utils::hash_fnv1a(game::il2cpp_string_chars(buttonName));
    for (auto &force_key : gsf::api_controls::flags_mouse)
    {
        if (force_key.game_identifier != hashed_btn_name)
            continue;

        if (force_key.ticks == 0)
            break;

        if (force_key.ticks > 0)
            --force_key.ticks;

        return true;
    }

    static auto o_fn = gsf::hooks::UnityEngine_Input_GetButton.get_original<decltype(hk_UnityEngine_Input_GetButton)>();
    return o_fn(buttonName);
}

utils::hook_detour gsf::hooks::UnityEngine_Input_GetButton(hk_UnityEngine_Input_GetButton);

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

game::sdk::Object *hk_UnityEngine_JsonUtility_FromJson(game::sdk::Il2CppString *json, game::sdk::Type *type)
{
    static auto o_fn = gsf::hooks::UnityEngine_JsonUtility_FromJson.get_original<decltype(hk_UnityEngine_JsonUtility_FromJson)>();
    return o_fn(json, type);
}

utils::hook_detour gsf::hooks::UnityEngine_JsonUtility_FromJson(hk_UnityEngine_JsonUtility_FromJson);

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

game::sdk::Il2CppString *hk_UnityEngine_JsonUtility_ToJson(game::sdk::Object *obj, game::sdk::Boolean prettyPrint)
{
    static auto o_fn = gsf::hooks::UnityEngine_JsonUtility_ToJson.get_original<decltype(hk_UnityEngine_JsonUtility_ToJson)>();
    auto result = o_fn(obj, prettyPrint);

    return result;
}

utils::hook_detour gsf::hooks::UnityEngine_JsonUtility_ToJson(hk_UnityEngine_JsonUtility_ToJson);

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK hk_WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (uMsg == WM_KEYDOWN && wParam == VK_DELETE)
        gsf::shutdown();

    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
        return TRUE;

    if ((uMsg == WM_KEYDOWN || uMsg == WM_KEYUP) && gsf::callback_manager::get_callbacks().on_key.dispatch_cancelable(uMsg == WM_KEYDOWN, wParam))
        return TRUE;

    if (gsf::menu::windowproc(uMsg, wParam, lParam))
        return TRUE;

    if (static auto &imgui_io = ImGui::GetIO(); imgui_io.WantCaptureKeyboard || imgui_io.WantCaptureMouse)
        return TRUE;

    static auto o_WindowProc = gsf::hooks::WindowProc.get_original<decltype(hk_WindowProc)>();
    return o_WindowProc(hwnd, uMsg, wParam, lParam);
}

utils::hook_wndproc gsf::hooks::WindowProc(hk_WindowProc);

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool gsf::hooks::install()
{
    DEBUG_CON_PRINT("\n[+] Installing hooks...");

	if (!DEBUG_CON_C_LOG(L"Initializing MinHook...", MH_Initialize() == MH_STATUS::MH_OK))
		return false;

    ID3D11DeviceContext *game_devicectx = *game::dx_devicectx_ptr;
    if (!DEBUG_CON_C_LOG(L"WindowProcedure Callback",         gsf::hooks::WindowProc                        .inhook(*game::window_handle_ptr))
    ||  !DEBUG_CON_C_LOG(L"ShowCursor",                       gsf::hooks::ShowCursor                        .inhook(::ShowCursor))
    ||  !DEBUG_CON_C_LOG(L"ID3D11DeviceContext::Draw",        gsf::hooks::Draw                              .inhook(GET_VFUNC_FROM_VCLASS_BY_IDX(game_devicectx,       0, Indexes_ID3D11DeviceContext::Draw)))
    ||  !DEBUG_CON_C_LOG(L"ID3D11DeviceContext::DrawIndexed", gsf::hooks::DrawIndexed                       .inhook(GET_VFUNC_FROM_VCLASS_BY_IDX(game_devicectx,       0, Indexes_ID3D11DeviceContext::DrawIndexed)))
    ||  !DEBUG_CON_C_LOG(L"IDXGISwapChain::ResizeBuffers",    gsf::hooks::ResizeBuffers                     .inhook(GET_VFUNC_FROM_VCLASS_BY_IDX(global::dx_swapchain, 0, Indexes_IDXGISwapChain::ResizeBuffers)))
    ||  !DEBUG_CON_C_LOG(L"IDXGISwapChain::Present",          gsf::hooks::Present                           .inhook(GET_VFUNC_FROM_VCLASS_BY_IDX(global::dx_swapchain, 0, Indexes_IDXGISwapChain::Present)))
    ||  !DEBUG_CON_C_LOG(L"UnityEngine_Cursor_set_lockState", gsf::hooks::UnityEngine_Cursor_set_lockState  .inhook(game::sdk::Cursor::lockState.setter.api_ptr()))
    ||  !DEBUG_CON_C_LOG(L"UnityEngine_Cursor_set_visible",   gsf::hooks::UnityEngine_Cursor_set_visible    .inhook(game::sdk::Cursor::visible.setter.api_ptr()))
    ||  !DEBUG_CON_C_LOG(L"UnityEngine_JsonUtility_ToJson",   gsf::hooks::UnityEngine_JsonUtility_ToJson    .inhook(game::sdk::JsonUtility::ToJson.api_ptr()))
    ||  !DEBUG_CON_C_LOG(L"UnityEngine_JsonUtility_FromJson", gsf::hooks::UnityEngine_JsonUtility_FromJson  .inhook(game::sdk::JsonUtility::FromJson.api_ptr()))
    ||  !DEBUG_CON_C_LOG(L"UnityEngine_Input_GetButton",      gsf::hooks::UnityEngine_Input_GetButton       .inhook(game::sdk::Input::GetButton.api_ptr()))
    ||  !DEBUG_CON_C_LOG(L"UnityEngine_Input_GetAxisRaw",     gsf::hooks::UnityEngine_Input_GetAxisRaw      .inhook(game::sdk::Input::GetAxisRaw.api_ptr()))
    // ||  !DEBUG_CON_C_LOG(L"UnityEngine_Animator_get_speed",   gsf::hooks::UnityEngine_Animator_get_speed    .inhook(game::sdk::Animator::_get_speed.api_ptr()))
    ) {
        return false;
    }

    // Setup for WorldEntityIterator_get_speed

    DEBUG_COUT("\n[+] Changing page protection for WorldEntityIterator_get_speed...");
    utils::change_page_protection WorldEntityIterator_get_speed_prot(gsf::hooks::WorldEntityIterator_get_speed_ptr, sizeof(WorldEntityIterator_get_speed_codeinj), PAGE_EXECUTE_READWRITE);
    if (!WorldEntityIterator_get_speed_prot)
        return false;

    DEBUG_COUT("\n[+] Backup original bytes for WorldEntityIterator_get_speed...");
    std::memcpy(WorldEntityIterator_get_speed_original, gsf::hooks::WorldEntityIterator_get_speed_ptr, sizeof(WorldEntityIterator_get_speed_original));

    *reinterpret_cast<void **>(WorldEntityIterator_get_speed_codeinj + 0x5) = WorldEntityIterator_get_speed_shim;
    *reinterpret_cast<std::int32_t *>(WorldEntityIterator_get_speed_codeinj + sizeof(WorldEntityIterator_get_speed_codeinj) - sizeof(std::int32_t)) = WorldEntityIterator_get_speed_codeinj_hotzone_size - sizeof(WorldEntityIterator_get_speed_codeinj);

    DEBUG_COUT("\n[+] Applying code injection for WorldEntityIterator_get_speed...");
    std::memcpy(gsf::hooks::WorldEntityIterator_get_speed_ptr, WorldEntityIterator_get_speed_codeinj, sizeof(WorldEntityIterator_get_speed_codeinj));

    return true;
}

bool gsf::hooks::uninstall()
{
	// Copy the entire list to a vector before iterating through and calling unhook.
	// This is done since we can't iterate the list while modifying it in the middle of an iteration.
	// Modifying it in terms of calling unhook which calls internal_on_unhook which then removes itself from the static instances std::list
	std::vector<utils::hook_base *> hook_instances { std::begin(utils::hook_base::instances), std::end(utils::hook_base::instances) };

	for (auto &hook_instance : hook_instances)
		if (!hook_instance->unhook())
			return false;

    DEBUG_COUT("\n[+] Changing page protection for WorldEntityIterator_get_speed...");
    utils::change_page_protection WorldEntityIterator_get_speed_prot(gsf::hooks::WorldEntityIterator_get_speed_ptr, sizeof(WorldEntityIterator_get_speed_codeinj), PAGE_EXECUTE_READWRITE);
    if (!WorldEntityIterator_get_speed_prot)
        return false;

    DEBUG_COUT("\n[+] Restoring original bytes for WorldEntityIterator_get_speed...");
    std::memcpy(gsf::hooks::WorldEntityIterator_get_speed_ptr, WorldEntityIterator_get_speed_original, sizeof(WorldEntityIterator_get_speed_original));

	return true;
}

#pragma warning (default: 26812)