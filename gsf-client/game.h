#pragma once

#include "sdk/sdk.h"

namespace game
{
	inline game::sdk::get_dx_swapchain_t get_dx_swapchain = nullptr; // TODO: grab the pointer manually instead of calling
	inline game::sdk::get_dx_devicectx_t get_dx_devicectx = nullptr; // TODO: grab the pointer manually instead of calling

	inline HWND                         *window_handle_ptr = nullptr;
	inline game::sdk::player_map_coords *player_map_coords = nullptr;

	inline game::sdk::unity_scripting_api<game::sdk::Boolean>                                                 UnityEngine_Cursor_get_visible              ("UnityEngine.Cursor::get_visible()");
	inline game::sdk::unity_scripting_api<void, game::sdk::Boolean>                                           UnityEngine_Cursor_set_visible              ("UnityEngine.Cursor::set_visible(System.Boolean)");
	inline game::sdk::unity_scripting_api<game::sdk::CursorLockMode>                                          UnityEngine_Cursor_get_lockState            ("UnityEngine.Cursor::get_lockState()");
	inline game::sdk::unity_scripting_api<void, game::sdk::CursorLockMode>                                    UnityEngine_Cursor_set_lockState            ("UnityEngine.Cursor::set_lockState(UnityEngine.CursorLockMode)");
	inline game::sdk::unity_scripting_api<game::sdk::Il2CppString *, game::sdk::Object *, game::sdk::Boolean> UnityEngine_JsonUtility_ToJson              ("UnityEngine.JsonUtility::ToJson(System.Object,System.Boolean)");
	inline game::sdk::unity_scripting_api<game::sdk::Object *, game::sdk::Il2CppString *, game::sdk::Type *>  UnityEngine_JsonUtility_FromJson            ("UnityEngine.JsonUtility::FromJson(System.String,System.Type)");
	inline game::sdk::unity_scripting_api<void, game::sdk::Il2CppString *, game::sdk::Object *>               UnityEngine_JsonUtility_FromJsonOverwrite   ("UnityEngine.JsonUtility::FromJsonOverwrite(System.String,System.Object)");
	inline game::sdk::unity_scripting_api<game::sdk::Boolean, game::sdk::Il2CppString *>                      UnityEngine_Input_GetButton                 ("UnityEngine.Input::GetButton(System.String)");
	inline game::sdk::unity_scripting_api<float, game::sdk::Il2CppString *>                                   UnityEngine_Input_GetAxisRaw                ("UnityEngine.Input::GetAxisRaw(System.String)");

	inline game::sdk::il2cpp_string_chars_t  il2cpp_string_chars  = nullptr;
	inline game::sdk::il2cpp_string_length_t il2cpp_string_length = nullptr;
	inline game::sdk::il2cpp_string_new_t    il2cpp_string_new    = nullptr;

	bool init();
}