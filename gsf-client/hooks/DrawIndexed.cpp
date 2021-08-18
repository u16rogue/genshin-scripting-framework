#include "hooks.h"
#include <d3d11.h>
#include <macro.h>

#include "../script_manager.h"

void hk_DrawIndexed(ID3D11DeviceContext *thisptr, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	for (auto &script : gsf::script_manager::get_scripts())
	{
		auto &callback = script->get_callbacks().dx_drawindexed;
		if (!callback.active)
			continue;

		auto args = script->get_lua().create_table_with("cancel", false, "IndexCount", IndexCount, "StartIndexLocation", StartIndexLocation, "BaseVertexLocation", BaseVertexLocation);

		callback.callback_function(args);

		if (args["cancel"] == true)
			return;
	}

	static auto o_DrawIndexed = hooks::ch_drawindexed->get_original<decltype(hk_DrawIndexed)>();
	return o_DrawIndexed(thisptr, IndexCount, StartIndexLocation, BaseVertexLocation);
}

std::unique_ptr<utils::hook_detour> hooks::ch_drawindexed = std::make_unique<utils::hook_detour>(hk_DrawIndexed);