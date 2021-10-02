#include "api_controls.h"

bool gsf::api_controls::setup_api(sol::state &slua)
{
	auto namespace_api = slua["controls"].get_or_create<sol::table>();
	namespace_api.set_function("mouse_down", &gsf::api_controls::mouse_down);
	return true;
}

void gsf::api_controls::mouse_down(int key, int ticks)
{
	// TODO: refactor this when we get more keys
	int *mkey_flags[] =
	{
		&api_controls::flag_mouse_left_down,
		&api_controls::flag_mouse_right_down,
		&api_controls::flag_mouse_middle_down
	};

	// Reset all keys
	if (key < 0)
	{
		for (auto key : mkey_flags)
			*key = 0;
		return;
	}

	// Invalid value
	if (key > (sizeof(mkey_flags) / sizeof(mkey_flags[0] - 1)))
		return;

	int &selected_mkey = *mkey_flags[key];

	// Only accept -1, 0, ... +inf
	if (ticks == 0 || ticks == -1)
		selected_mkey = ticks;
	else if (selected_mkey < ticks)
		selected_mkey = ticks;
}
