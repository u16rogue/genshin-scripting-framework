#include "hooks.h"
#include <d3d11.h>
#include <macro.h>

void hk_DrawIndexed(ID3D11DeviceContext *thisptr, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	// DEBUG_COUT("\nID3D11DeviceContext::DrawIndexed was called!");

	static auto o_DrawIndexed = hooks::ch_drawindexed->get_original<decltype(hk_DrawIndexed)>();
	return o_DrawIndexed(thisptr, IndexCount, StartIndexLocation, BaseVertexLocation);
}

std::unique_ptr<utils::hook_detour> hooks::ch_drawindexed = std::make_unique<utils::hook_detour>(hk_DrawIndexed);