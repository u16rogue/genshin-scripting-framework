#include "hooks.h"

#include <macro.h>
#include <Windows.h>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include <macro.h>
#include "../gsf_client.h"
#include "../features/fps_counter.h"

ID3D11DeviceContext    *dx_context            = nullptr;
ID3D11RenderTargetView *dx_render_target_view = nullptr;

HRESULT __stdcall hk_Present(IDXGISwapChain *thisptr, UINT SyncInterval, UINT Flags)
{
    static bool init_success = [&, thisptr]() -> bool
    {
        ID3D11Device *dx_device = nullptr;
        if (!DEBUG_CON_C_LOG(L"Get device", SUCCEEDED(thisptr->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void **>(&dx_device)))))
            return false;

        dx_device->GetImmediateContext(&dx_context);
        DEBUG_CON_C_LOG(L"Get context", dx_context != nullptr);

        ID3D11Texture2D *dx_backbuffer = nullptr;
        if (!DEBUG_CON_C_LOG(L"Get buffer", SUCCEEDED(thisptr->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&dx_backbuffer)))))
            return false;

        if (!DEBUG_CON_C_LOG(L"Create render target", SUCCEEDED(dx_device->CreateRenderTargetView(dx_backbuffer, nullptr, &dx_render_target_view))))
            return false;

        dx_backbuffer->Release();

        DXGI_SWAP_CHAIN_DESC scd;
        if (!DEBUG_CON_C_LOG(L"Get swap chain description", SUCCEEDED(thisptr->GetDesc(&scd))))
            return false;

        if (!DEBUG_CON_C_LOG(L"Initialize ImGui implementation", ImGui_ImplWin32_Init(scd.OutputWindow) && ImGui_ImplDX11_Init(dx_device, dx_context)))
            return false;

        return true;
    }();

    static auto o_Present = gsf::hooks::DX_Present.get_original<decltype(hk_Present)>();

    if (!init_success)
        return o_Present(thisptr, SyncInterval, Flags);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    gsf::render_imgui();


    ImGui::Render();

    dx_context->OMSetRenderTargets(1, &dx_render_target_view, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    auto result = o_Present(thisptr, SyncInterval, Flags);

    gsf::features::fps_counter::on_present();

    return result;
}

utils::hook_detour gsf::hooks::DX_Present(hk_Present);