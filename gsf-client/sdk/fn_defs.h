#pragma once

#include "enums.h"

namespace game::sdk
{
	using get_fn_t = void *(__fastcall *)(const char *);
	using UnityEngine_Cursor_get_visible_t = int(__fastcall*)(void);
	using UnityEngine_Cursor_set_visible_t = void(__fastcall*)(int);
	using UnityEngine_Cursor_get_lockState_t = game::sdk::CursorLockMode(__fastcall*)(void);
	using UnityEngine_Cursor_set_lockState_t = void(__fastcall*)(CursorLockMode);
}