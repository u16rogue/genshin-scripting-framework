#pragma once

#include <imgui.h>
#include <string>
#include <vector>

namespace helpers
{
	class imgui_popup_modal
	{
		using callback_t = void(*)(void);
	public:
		imgui_popup_modal(std::string_view str_, callback_t callback_);

		void show();
		
	private:
		bool prompt_requested = false;
		std::string str;
		callback_t callback;

	private:
		inline static std::vector<helpers::imgui_popup_modal *> instances;

	public:
		// Used to draw the popup modal instances, call on imgui draw
		// This is static and must be called as is, this implements all the registered instances of a popup modal
		static void on_imgui_draw();
	};
}