#pragma once

#include "sdk/sdk.h"

namespace game
{
	inline game::sdk::player_map_coords *player_map_coords = nullptr;

	inline game::sdk::UnityEngine_Cursor_get_visible_t            engine_cursor_get_visible      ("UnityEngine.Cursor::get_visible()");
	inline game::sdk::UnityEngine_Cursor_set_visible_t            engine_cursor_set_visible      ("UnityEngine.Cursor::set_visible(System.Boolean)");
	inline game::sdk::UnityEngine_Cursor_get_lockState_t          engine_cursor_get_lockstate    ("UnityEngine.Cursor::get_lockState()");
	inline game::sdk::UnityEngine_Cursor_set_lockState_t          engine_cursor_set_lockstate    ("UnityEngine.Cursor::set_lockState(UnityEngine.CursorLockMode)");
	inline game::sdk::UnityEngine_JsonUtility_ToJson_t            engine_jsonutility_tojson      ("UnityEngine.JsonUtility::ToJson(System.Object,System.Boolean)");
	inline game::sdk::UnityEngine_JsonUtility_FromJson_t          engine_jsonutility_fromjson    ("UnityEngine.JsonUtility::FromJson(System.String,System.Type)");
	inline game::sdk::UnityEngine_JsonUtility_FromJsonOverwrite_t engine_jsonutility_fromjsonovr ("UnityEngine.JsonUtility::FromJsonOverwrite(System.String,System.Object)");
	inline game::sdk::UnityEngine_Input_GetButton_t               engine_input_get_button        ("UnityEngine.Input::GetButton(System.String)");
	inline game::sdk::UnityEngine_Input_GetAxisRaw_t              engine_input_get_axis_raw      ("UnityEngine.Input::GetAxisRaw(System.String)");

	inline game::sdk::il2cpp_string_chars_t  il2cpp_string_chars  = nullptr;
	inline game::sdk::il2cpp_string_length_t il2cpp_string_length = nullptr;
	inline game::sdk::il2cpp_string_new_t    il2cpp_string_new    = nullptr;

	bool init();
}