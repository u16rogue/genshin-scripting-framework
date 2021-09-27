#pragma once

#include "cs_types.h"

namespace game::sdk
{
	enum class CursorLockMode : int
	{
		None,
		Locked,
		Confined
	};

	using get_fn_t                           = void *(*)(const char *);
	using UnityEngine_Cursor_get_visible_t   = CS_Boolean_t(*)(void);
	using UnityEngine_Cursor_set_visible_t   = void(*)(CS_Boolean_t);
	using UnityEngine_Cursor_get_lockState_t = game::sdk::CursorLockMode(*)(void);
	using UnityEngine_Cursor_set_lockState_t = void(*)(CursorLockMode);
}