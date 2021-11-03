#pragma once

#include "unity_component.h"

namespace game::sdk
{
	enum class MonoOrStereoscopicEye : int
	{
		Left,
		Right,
		Mono
	};

	class Camera : public game::sdk::Component
	{
	public:
		void WorldToScreenPoint_inplace(game::sdk::Vector3 &worldpoint)
		{
			this->INTERNAL_CALL_WorldToScreenPoint(this, &worldpoint, false, &worldpoint);
		}

		game::sdk::Vector3 WorldToScreenPoint(game::sdk::Vector3 worldpoint)
		{
			game::sdk::Vector3 out;
			this->INTERNAL_CALL_WorldToScreenPoint(this, &out, false, &worldpoint);
			return out;
		}

	public:
		inline static auto INTERNAL_CALL_WorldToScreenPoint = game::sdk::unity_scripting_api<void, game::sdk::Camera *, game::sdk::Vector3 *, bool, game::sdk::Vector3 *>("UnityEngine.Camera::INTERNAL_CALL_WorldToScreenPoint(UnityEngine.Camera,UnityEngine.Vector3&,System.Boolean,UnityEngine.Vector3&)");
		inline static auto main            = game::sdk::unity_setter_getter_static<game::sdk::Camera *>("UnityEngine.Camera::get_main()", nullptr);
		inline static auto current         = game::sdk::unity_setter_getter_static<game::sdk::Camera *>("UnityEngine.Camera::get_current()", nullptr);
		inline static auto allCamerasCount = game::sdk::unity_setter_getter_static<int>("UnityEngine.Camera::get_allCamerasCount()", nullptr);
	};

	union Camera_class_unk1
	{
		struct
		{
			char _pad0[0x8];
			game::sdk::Camera *current_camera;
		};
	};

	union Camera_class_unk0
	{
		struct
		{
			char _pad0[0xA0];
			Camera_class_unk1 *camera_container_maybe;
		};

		struct
		{
			char _pad1[0x10A];
			bool check1; // must be false
		};

		struct
		{
			char _pad2[0xBC];
			bool check2; // must be false
		};
	};
}

GSF_UNITY_SDK_ENSURE_NO_NONSTATIC(game::sdk::Camera);