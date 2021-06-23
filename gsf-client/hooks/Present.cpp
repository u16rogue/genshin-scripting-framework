#include "hooks.h"

#include <macro.h>

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
        ID3D11Texture2D *dx_backbuffer;
        if (FAILED(thisptr->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&dx_backbuffer))))
            return false;

        DEBUG_WCOUT("\nhooks::hk_Present # initialization # Create render target");
        dx_device->CreateRenderTargetView(dx_backbuffer, nullptr, &dx_render_target_view);
        dx_backbuffer->Release();

        DEBUG_WCOUT("\nhooks::hk_Present # initialization # Create ImGui context");
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        ImGui::StyleColorsDark();

        DEBUG_WCOUT("\nhooks::hk_Present # initialization # Get swap chain description");
        DXGI_SWAP_CHAIN_DESC scd;
        thisptr->GetDesc(&scd);

        DEBUG_WCOUT("\nhooks::hk_Present # initialization # Initialize ImGui implementation");
        ImGui_ImplWin32_Init(scd.OutputWindow);
        ImGui_ImplDX11_Init(dx_device, dx_context);

        DEBUG_WCOUT("\nhooks::hk_Present # initialization # Initialization succesful!");
        return true;
    }();

    if (!init_success)
        return hooks::ch_present->get_original<decltype(hk_Present)>()(thisptr, SyncInterval, Flags);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar())
    {
        ImGui::Text("Genshin Scripting Framework");
    }
    ImGui::EndMainMenuBar();

    ImGui::Render();

    dx_context->OMSetRenderTargets(1, &dx_render_target_view, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return hooks::ch_present->get_original<decltype(hk_Present)>()(thisptr, SyncInterval, Flags);
}

std::unique_ptr<utils::hook_vmt> hooks::ch_present = std::make_unique<utils::hook_vmt>(8, hk_Present);