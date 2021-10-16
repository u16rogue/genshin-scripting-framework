#include "hooks.h"

#include <d3d11.h>
#include "../script_manager.h"
#include "../callback_manager.h"

void __stdcall hk_Draw(ID3D11DeviceContext *thisptr, UINT VertexCount, UINT StartVertexLocation)
{
	if (gsf::callback_manager::get_callbacks().dx_draw.dispatch_cancellable(VertexCount, StartVertexLocation))
		return;

	static auto o_Draw = gsf::hooks::Draw.get_original<decltype(hk_Draw)>();
	return o_Draw(thisptr, VertexCount, StartVertexLocation);
}

utils::hook_detour gsf::hooks::Draw(hk_Draw);