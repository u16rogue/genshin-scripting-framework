#include "hooks.h"

#include <d3d11.h>
#include <macro.h>

void __stdcall hk_Draw(ID3D11DeviceContext *thisptr, UINT VertexCount, UINT StartVertexLocation)
{
	// DEBUG_COUT("\nID3D11DeviceContext::Draw was called!");

	static auto o_Draw = hooks::ch_draw->get_original<decltype(hk_Draw)>();
	return o_Draw(thisptr, VertexCount, StartVertexLocation);
}

std::unique_ptr<utils::hook_detour> hooks::ch_draw = std::make_unique<utils::hook_detour>(hk_Draw);