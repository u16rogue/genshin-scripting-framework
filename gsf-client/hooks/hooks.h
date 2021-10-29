#pragma once

#include <hooking.h>

// TODO: decide whether to merge to one file?

namespace gsf::hooks
{
	extern utils::hook_detour  Present;
	extern utils::hook_detour  ResizeBuffers;
	extern utils::hook_wndproc WindowProc;
	extern utils::hook_detour  Draw;
	extern utils::hook_detour  DrawIndexed;
	extern utils::hook_detour  ShowCursor;
	extern utils::hook_detour  UnityEngine_Cursor_set_lockState;
	extern utils::hook_detour  UnityEngine_Cursor_set_visible;
	extern utils::hook_detour  UnityEngine_JsonUtility_ToJson;
	extern utils::hook_detour  UnityEngine_JsonUtility_FromJson;
	extern utils::hook_detour  UnityEngine_Input_GetButton;
	extern utils::hook_detour  UnityEngine_Input_GetAxisRaw;
	extern utils::hook_detour  UnityEngine_Animator_get_speed;

	bool install();
	bool uninstall();
}