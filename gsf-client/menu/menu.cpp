#include "menu.h"
#include "../game.h"
#include "../git_info.h"
#include "../gsf_client.h"
#include "../script_manager.h"
#include "../callback_manager.h"
#include <imgui.h>

#include "tab_scripts.h"
#include "tab_logs.h"
#include "tab_about.h"
#include "tab_misc.h"

static bool internal_window_vis_state = gsf::menu::is_open;

static void set_open_state(bool open)
{
	static int last_visible;
	static game::sdk::CursorLockMode last_lock;

	internal_window_vis_state = gsf::menu::is_open = open;

	if (gsf::menu::is_open)
	{
		last_visible = game::sdk::Cursor::visible;
		last_lock    = game::sdk::Cursor::lockState;
	}

	game::sdk::Cursor::visible   = gsf::menu::is_open ? true : last_visible;
	game::sdk::Cursor::lockState = gsf::menu::is_open ? game::sdk::CursorLockMode::None : last_lock;
}

bool gsf::menu::windowproc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KEYDOWN && wParam == VK_INSERT)
		set_open_state(!gsf::menu::is_open);

	return gsf::menu::is_open;
}

void gsf::menu::render_imgui()
{
	gsf::menu::tab_misc::render_window();
	gsf::menu::tab_logs::render_window();

	if (!gsf::menu::is_open)
		return;

	if (!internal_window_vis_state)
	{
		set_open_state(false);
		return;
	}

	static const auto _set_wh = []()
	{
		const auto display_wh = ImGui::GetIO().DisplaySize;
		ImGui::SetNextWindowSize({ display_wh.x * 0.8f, display_wh.y * 0.6f }, ImGuiCond_::ImGuiCond_FirstUseEver);
		return true;
	}();

	if (ImGui::Begin("Genshin Impact Scripting Framework | " GIT_BRANCH " @ " GIT_HASH, &internal_window_vis_state, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar))
	{
		// Menu bar
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::MenuItem("Shutdown (DEL)"))
				gsf::shutdown();

			ImGui::EndMenuBar();
		}

		// Tabs
		if (ImGui::BeginTabBar("##gsf_tabs"))
		{
			tab_scripts::render_tab();
			tab_misc::render_tab();
			tab_logs::render_tab();

			// Render custom script tabs
			for (const auto &cbs : gsf::callback_manager::get_callbacks().menu_imgui_tab.lua_callbacks)
			{
				if (!cbs.active)
					continue;

				if (ImGui::BeginTabItem(cbs.parent->get_config().name.c_str()))
				{
					cbs.lua_func();
					ImGui::EndTabItem();
				}
			}

			tab_about::render_tab();
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}