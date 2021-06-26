#include "hooks.h"

#include <macro.h>
#include <Windows.h>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include "../gsf_client.h"

ID3D11DeviceContext    *dx_context            = nullptr;
ID3D11RenderTargetView *dx_render_target_view = nullptr;

HRESULT __stdcall hk_Present(IDXGISwapChain *thisptr, UINT SyncInterval, UINT Flags)
{
    static bool init_success = [&, thisptr]() -> bool
    {
        DEBUG_WCOUT("\nhooks::hk_Present # initialization # Get device");
        ID3D11Device *dx_device = nullptr;
        if (FAILED(thisptr->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void **>(&dx_device))))
            return false;

        DEBUG_WCOUT("\nhooks::hk_Present # initialization # Get context");
        dx_device->GetImmediateContext(&dx_context);

        DEBUG_WCOUT("\nhooks::hk_Present # initialization # Get buffer");
        ID3D11Texture2D *dx_backbuffer = nullptr;
        if (FAILED(thisptr->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&dx_backbuffer))))
            return false;

        DEBUG_WCOUT("\nhooks::hk_Present # initialization # Create render target");
        dx_device->CreateRenderTargetView(dx_backbuffer, nullptr, &dx_render_target_view);
        dx_backbuffer->Release();

        DEBUG_WCOUT("\nhooks::hk_Present # initialization # Get swap chain description");
        DXGI_SWAP_CHAIN_DESC scd;
        thisptr->GetDesc(&scd);

        DEBUG_WCOUT("\nhooks::hk_Present # initialization # Initialize ImGui implementation");
        ImGui_ImplWin32_Init(scd.OutputWindow);
        ImGui_ImplDX11_Init(dx_device, dx_context);

        DEBUG_WCOUT("\nhooks::hk_Present # initialization # Initialization succesful!");
        return true;
    }();

    static auto o_Present = hooks::ch_present->get_original<decltype(hk_Present)>();

    if (!init_success)
        return o_Present(thisptr, SyncInterval, Flags);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    gsf::render_imgui();

    ImGui::Render();

    dx_context->OMSetRenderTargets(1, &dx_render_target_view, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return o_Present(thisptr, SyncInterval, Flags);
}

std::unique_ptr<utils::hook_detour> hooks::ch_present = std::make_unique<utils::hook_detour>(hk_Present);