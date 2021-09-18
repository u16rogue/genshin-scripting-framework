#pragma once

#include "i_api.h"

namespace gsf
{
	class api_imgui : public i_api
	{
	protected:
		api_imgui() {}
		bool setup_api(sol::state &slua);

	public:
		std::int64_t imgui_active_begin_count = 0;

	private:
		bool _api_begin(const char *text);
		void _api_iend();
	};
}
