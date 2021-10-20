#include "hooks.h"
#include <d3d11.h>
#include "../script_manager.h"
#include "../callback_manager.h"

void __stdcall hk_DrawIndexed(ID3D11DeviceContext *thisptr, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	if (gsf::callback_manager::get_callbacks().dx_drawindexed.dispatch_cancelable(IndexCount, StartIndexLocation, BaseVertexLocation))
		return;

	static auto o_DrawIndexed = gsf::hooks::DrawIndexed.get_original<decltype(hk_DrawIndexed)>();
	return o_DrawIndexed(thisptr, IndexCount, StartIndexLocation, BaseVertexLocation);
}

utils::hook_detour gsf::hooks::DrawIndexed(hk_DrawIndexed);