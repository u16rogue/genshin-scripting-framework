#include "api_controls.h"

bool gsf::api_controls::setup_api(sol::state &slua)
{
	auto namespace_controls = slua["controls"].get_or_create<sol::table>();

	#define _ENUM_ENTRY_api_controls(_enum) #_enum ## , ## api_controls::keys:: ## _enum
	namespace_controls.new_enum("key",
		_ENUM_ENTRY_api_controls(LMOUSE),
		_ENUM_ENTRY_api_controls(ATTACK),

		_ENUM_ENTRY_api_controls(RMOUSE),
		_ENUM_ENTRY_api_controls(SPRINT),

		_ENUM_ENTRY_api_controls(MMOUSE),
		_ENUM_ENTRY_api_controls(ELEMENTAL_SIGHT),
		_ENUM_ENTRY_api_controls(RESET_CAMERA)
	);
	#undef _ENUM_ENTRY_api_controls

	namespace_controls.set_function("mouse_down", &gsf::api_controls::mouse_down);
	return true;
}

void gsf::api_controls::mouse_down(api_controls::keys key, int ticks)
{
	int *selected_key = nullptr;

	switch (key)
	{
		case gsf::api_controls::keys::LMOUSE:
		{
			selected_key = &api_controls::flag_mouse_left_down;
			break;
		}
		case gsf::api_controls::keys::RMOUSE:
		{
			selected_key = &api_controls::flag_mouse_right_down;
			break;
		}
		case gsf::api_controls::keys::MMOUSE:
		{
			selected_key = &api_controls::flag_mouse_middle_down;
			break;
		}

		default:
			return;
	}

	// we're using decltype of variable ticks for consistency, int32's -1 and int64's -1 have different bits
	// this is for future proofing incase a refactoring would take place
	constexpr decltype(ticks) TICKS_HOLD_KEY    = -1;
	constexpr decltype(ticks) TICKS_RELEASE_KEY =  0;
	if (ticks == TICKS_RELEASE_KEY || ticks == TICKS_HOLD_KEY)
	{
		*selected_key = ticks;
	}
	else if (ticks > *selected_key) // only accept new n ticks if its higher than the current tick count for the key
	{
		*selected_key = ticks;
	}
}
