#pragma once

namespace game::sdk
{
	enum class CursorLockMode : int
	{
		None,
		Locked,
		Confined
	};

	using get_fn_t                           = void *(*)(const char *);
	using UnityEngine_Cursor_get_visible_t   = int(*)(void);
	using UnityEngine_Cursor_set_visible_t   = void(*)(int);
	using UnityEngine_Cursor_get_lockState_t = game::sdk::CursorLockMode(*)(void);
	using UnityEngine_Cursor_set_lockState_t = void(*)(CursorLockMode);
}