#pragma once

#define GSF_DEF_GAME_WND_CLASS "UnityWndClass"
#define GSF_DEF_GAME_WND_TITLE "Genshin Impact"

#define GSF_DEF_METACONFIG_NAME "_mconfig"

namespace gsf::def::vtidx
{
	enum class IDXGISwapChain
	{
		Present = 8,
	};

	enum class ID3D11DeviceContext
	{
		DrawIndexed = 12,
		Draw = 13
	};
}