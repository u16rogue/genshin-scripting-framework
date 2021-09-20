#include "menu.h"

bool gsf::menu::windowproc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KEYDOWN && wParam == VK_INSERT)
	{
		gsf::menu::is_open = !gsf::menu::is_open;
		if (gsf::menu::is_open)
		{
			ShowCursor(TRUE);
			ClipCursor(nullptr);
		}
	}
	return gsf::menu::is_open;
}

void gsf::menu::render_imgui()
{
	if (!gsf::menu::is_open)
		return;

	if (ImGui::Begin("Genshin Impact Scripting Framework"))
	{
		ImGui::Text("Hello world!");
	}
	ImGui::End();
}