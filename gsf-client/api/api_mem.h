#pragma once

#include "i_api.h"

namespace gsf
{
	class api_mem : public i_api
	{
	protected:
		api_mem() {}
		bool setup_api(sol::state &slua);

	private:
		sol::object _api_ida_scan(std::uintptr_t start_adr, std::size_t size, std::string ida_pattern);
		int _api_patch(std::uintptr_t addr, std::vector<std::uint8_t> byte_array);
		std::uint64_t _api_read_uint(std::uintptr_t addr, std::size_t prim_t_size);
		void _api_write_uint(std::uintptr_t addr, std::size_t prim_t_size, std::uint64_t value);
		std::uintptr_t _api_calc_rel_address_32(std::uintptr_t inst_addr, std::size_t inst_size);
	};
}