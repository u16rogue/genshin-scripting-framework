#include "api_controls.h"

#include <macro.h>

bool gsf::api_controls::setup_api(sol::state &slua)
{
	auto namespace_controls = slua["controls"].get_or_create<sol::table>();

	#define _ENUM_ENTRY_api_controls(_e, _v) #_v ## , ## api_controls:: ## _e ## :: ## _v

	namespace_controls.new_enum("keys",
		_ENUM_ENTRY_api_controls(keys, LMOUSE),
		_ENUM_ENTRY_api_controls(keys, ATTACK),

		_ENUM_ENTRY_api_controls(keys, RMOUSE),
		_ENUM_ENTRY_api_controls(keys, SPRINT),

		_ENUM_ENTRY_api_controls(keys, MMOUSE),
		_ENUM_ENTRY_api_controls(keys, ELEMENTAL_SIGHT),
		_ENUM_ENTRY_api_controls(keys, RESET_CAMERA)
	);

	namespace_controls.new_enum("directions",
		_ENUM_ENTRY_api_controls(directions, UP),
		_ENUM_ENTRY_api_controls(directions, DOWN),
		_ENUM_ENTRY_api_controls(directions, LEFT),
		_ENUM_ENTRY_api_controls(directions, RIGHT)
	);

	#undef _ENUM_ENTRY_api_controls

	namespace_controls.set_function("key_down", &gsf::api_controls::key_down);
	namespace_controls.set_function("look", &gsf::api_controls::look);

	return true;
}

void gsf::api_controls::key_down(api_controls::keys key, int ticks)
{
	for (auto &flag_key : api_controls::flags_mouse)
	{
		if (flag_key.enum_tag != key)
			continue;

		// we're using decltype of variable ticks for consistency, int32's -1 and int64's -1 have different bits
		// this is for future proofing incase a refactoring would take place
		constexpr decltype(ticks) TICKS_HOLD_KEY    = -1;
		constexpr decltype(ticks) TICKS_RELEASE_KEY =  0;
		if (ticks == TICKS_RELEASE_KEY || ticks == TICKS_HOLD_KEY || ticks > flag_key.ticks /* only accept new n ticks if its higher than the current tick count for the key */ )
			flag_key.ticks = ticks;

		return;
	}
}

void gsf::api_controls::look(api_controls::directions direction, float amount)
{
	constexpr int SHOULD_BE_NEGATIVE = (int)api_controls::directions::LEFT ^ (int)api_controls::directions::RIGHT ^ (int)api_controls::directions::UP ^ (int)api_controls::directions::DOWN;

	for (auto &flag_direction : api_controls::flags_look)
	{
		if (~(int)flag_direction.enum_tag & (int)direction)
			continue;

		flag_direction.amount = amount * ((int)direction & SHOULD_BE_NEGATIVE ? -1.f : 1.f);

		return;
	}
}
