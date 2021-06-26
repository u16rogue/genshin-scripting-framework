#pragma once

#include <singleton.h>
#include <cstdint>
#include <cstddef>
#include <string>

namespace gsf::features
{
	class fps_counter : public utils::impl_singleton<fps_counter>
	{
		static constexpr int total_delta = 1000; // 1000ms = 1s

	public:
		fps_counter() = default;

		void on_present();
		void on_imgui_draw();

		bool active = true;

	private:
		std::size_t current_accumilation = 0;
		std::size_t total_accumilation   = 0;
		std::size_t last_accumilation    = 0;
		std::size_t next_tick_accumilate = 0;

		std::string fps_str;
	};
}