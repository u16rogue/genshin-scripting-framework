#pragma once

#include "i_api.h"
#include <unordered_map>

namespace gsf
{
	class api_imgui : public i_api
	{
	protected:
		api_imgui() {}
		bool setup_api(sol::state &slua);

	public:
		inline static std::int64_t imgui_active_begin_count = 0;

	private:
		using data_ref_t = char[128];
		std::unordered_map<const char *, data_ref_t> imgui_data_ref;

	private:

		template <typename T>
		void _api_set_data_ref(const char *label, T value)
		{
			*reinterpret_cast<T*>(&this->imgui_data_ref[label]) = value;
		};

		template <>
		void _api_set_data_ref(const char *label, const char *value)
		{
			strncpy_s(this->imgui_data_ref[label], value, sizeof(data_ref_t));
		};

		bool _api_begin(const char *text);
		void _api_iend();
		bool _api_check_box(const char *label);
		const char *_api_input_text(const char *label);
		float _api_slider_float(const char *label, float min, float max);
	};
}
