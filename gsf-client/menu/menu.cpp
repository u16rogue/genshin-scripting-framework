#include "menu.h"
#include "../game.h"
#include "../git_info.h"

bool gsf::menu::windowproc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KEYDOWN && wParam == VK_INSERT)
	{
		gsf::menu::is_open = !gsf::menu::is_open;

		game::engine_cursor_set_visible(gsf::menu::is_open);
		game::engine_cursor_set_lockstate(gsf::menu::is_open ? game::sdk::CursorLockMode::None : game::sdk::CursorLockMode::Locked);
	}
	return gsf::menu::is_open;
}

void gsf::menu::render_imgui()
{
	if (!gsf::menu::is_open)
		return;

	if (ImGui::Begin("Genshin Impact Scripting Framework | " GIT_BRANCH " @ " GIT_HASH))
	{

	}
	ImGui::End();
}