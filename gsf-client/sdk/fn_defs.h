#pragma once

#include "enums.h"

namespace game::sdk
{
	using get_fn_t = void *(__fastcall *)(const char *name);
	using UnityEngine_Cursor_set_visible_t = void(__fastcall*)(int);
	using UnityEngine_Cursor_set_lockState_t = void(__fastcall*)(CursorLockMode);
}