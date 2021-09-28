#pragma once

#include <cstdint>
#include <cstddef>

namespace utils
{
	std::uint8_t *aob_scan(void *start, std::size_t size, const char *signature, const char *mask);
	std::uint8_t *aob_scan(void *proc_handle, void *start, std::size_t size, const char *signature, const char *mask);

	std::uint8_t *ida_scan(void *start, std::size_t size, const char *signature);

	template <typename T = std::uint8_t>
	T *aob_scan(void *start, std::size_t size, const char *signature, const char *mask)
	{
		return reinterpret_cast<T *>(utils::aob_scan(start, size, signature, mask));
	}

	std::uint8_t *calc_rel2abs32(void *instruction_address, std::size_t instruction_size);
}