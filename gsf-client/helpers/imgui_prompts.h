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
		
		void __on_imgui_draw(); // FOR IMPLEMENTATION USE ONLY

	private:
		bool prompt_requested = false;
		std::string str;
		callback_t callback;

	public:
		inline static std::vector<helpers::imgui_popup_modal *> instances;
	};
}