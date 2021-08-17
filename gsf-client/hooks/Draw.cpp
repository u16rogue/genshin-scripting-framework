#include "hooks.h"

#include <d3d11.h>
#include <macro.h>

#include "../script_manager.h"

void __stdcall hk_Draw(ID3D11DeviceContext *thisptr, UINT VertexCount, UINT StartVertexLocation)
{
	for (auto &script : gsf::script_manager::get_scripts())
	{
		auto &callback = script.get_callbacks().dx_draw;
		if (!callback.active)
			continue;

		auto args = script.get_lua().create_table_with("cancel", false, "VertexCount", VertexCount, "StartVertexLocation", StartVertexLocation);

		callback.callback_function(args);

		if (args["cancel"] == true)
			return;
	}

	static auto o_Draw = hooks::ch_draw->get_original<decltype(hk_Draw)>();
	return o_Draw(thisptr, VertexCount, StartVertexLocation);
}

std::unique_ptr<utils::hook_detour> hooks::ch_draw = std::make_unique<utils::hook_detour>(hk_Draw);