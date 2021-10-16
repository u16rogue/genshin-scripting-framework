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
		std::unordered_map<const char *, char[128]> imgui_data_ref;

	private:
		bool _api_begin(const char *text);
		void _api_iend();
		bool _api_check_box(const char *label);
		const char *_api_input_text(const char *label);
	};
}
